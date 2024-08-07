#include <cassert>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "compiler/Compiler.hpp"
#include "compiler/Errors.hpp"
#include "lexer/Token.hpp"
#include "logging/Logger.hpp"
#include "parser/AndTest.hpp"
#include "parser/Constructor.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

std::unique_ptr<Expression> Expression::parse(Lexer &lexer) {
  std::unique_ptr<Expression> expression;

  switch (lexer.peek()->type) {
    using enum Token::Type;
  case IF:
    expression = CondExpression::parse(lexer);
    break;

  case INT_LITERAL:
  case FLOAT_LITERAL:
  case CHAR_LITERAL:
  case STRING_LITERAL:
  case NAME:
  case PLUS:
  case MINUS:
  case MULT:
  case DIV:
  case NOT:
  case TRUE:
  case FALSE:
  case LPAREN:
  case LBRACKET:
    expression = NormalExpression::parse(lexer);
    break;

  case FN:
    expression = FunctionExpression::parse(lexer);
    break;

  default:
    AstNode::parser_error(
        std::string("unexpected token '") + lexer.peek()->to_string_short() + '\'', lexer);
  }

  return expression;
}

void CondExpression::parse_test_body(Lexer &lexer) {
  Logger::log_with_indent("<cond-test-body>");

  std::vector<std::unique_ptr<Statement>> body;
  while (true) {
    std::optional<std::unique_ptr<Statement>> statement = Statement::try_parse_statement(lexer);
    if (statement.has_value())
      body.push_back(std::move(statement.value()));
    else
      break;
  }

  if (!body.back()->is_expression())
    AstNode::parser_error(
        "missing return, last statement in each cond branch must be an expression", lexer);

  this->bodies.push_back(std::move(body));

  Logger::log_with_dedent("</cond-test-body>");
}

void CondExpression::parse_if(Lexer &lexer) {
  Logger::log_with_indent("<cond-if>");

  lexer.skip({Token::Type::IF});
  this->tests.push_back(Expression::parse(lexer));
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("</cond-if>");
}

void CondExpression::parse_elif(Lexer &lexer) {
  Logger::log_with_indent("<cond-elif>");

  lexer.skip({Token::Type::ELIF});
  this->tests.push_back(Expression::parse(lexer));
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("<cond-elif/>");
}

void CondExpression::parse_elifs(Lexer &lexer) {
  Logger::log_with_indent("<cond-elifs>");

  while (lexer.peek()->type == Token::Type::ELIF)
    this->parse_elif(lexer);

  Logger::log_with_dedent("</cond-elifs>");
}

void CondExpression::parse_else(Lexer &lexer) {
  Logger::log_with_indent("<cond-else>");

  lexer.skip({Token::Type::ELSE});
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("</cond-else>");
}

std::unique_ptr<CondExpression> CondExpression::parse(Lexer &lexer) {
  auto expression = std::make_unique<CondExpression>();
  expression->start_parsing(lexer, "<cond-expression>");

  expression->parse_if(lexer);
  expression->parse_elifs(lexer);
  expression->parse_else(lexer);

  expression->finish_parsing(lexer, "</cond-expression>");
  return expression;
}

// Compile the body of the branch and ensure local variables are scoped correctly
static llvm::Value *compile_branch_body(Compiler &compiler,
                                        const std::vector<std::unique_ptr<Statement>> &body) {
  for (size_t j = 0; j < body.size() - 1; ++j)
    body[j]->compile(compiler);

  return body.back()->compile(compiler);
}

// TODO: this whole function is way too big
llvm::Value *CondExpression::compile(Compiler &compiler) const {
  llvm::Function *current_function = compiler.get_current_function();

  llvm::BasicBlock *branch = compiler.create_basic_block(current_function, "if_branch");
  llvm::BasicBlock *merge_branch = compiler.create_basic_block(current_function, "merge_branch");

  // llvm does not allow 2 labels in a row so we need to make a branch here in case there are no
  // preceding instructions in the function
  compiler.create_branch(branch);

  // Vector of possible incoming values to the phi node (the return value of the cond expression)
  std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> incoming_values;

  // if/elif branches (each branch branches to the merge branch if true, next elif branch (or else
  // branch for last elif) if false)
  size_t num_tests = this->tests.size();
  for (size_t i = 0; i < num_tests; ++i) {
    compiler.set_insert_point(branch);
    llvm::Value *test = this->tests[i]->compile(compiler);

    std::variant<llvm::Value *, BinaryOperatorError> test_is_true =
        compiler.create_eq(test, compiler.create_bool(true));
    if (std::holds_alternative<BinaryOperatorError>(test_is_true))
      this->compiler_error(std::get<BinaryOperatorError>(test_is_true));

    std::string branch_name = (i == num_tests - 1) ? "else_branch" : "elif_branch";

    llvm::BasicBlock *next_branch = compiler.create_basic_block(current_function, branch_name);

    compiler.start_scope();
    llvm::Value *current_return_value = compile_branch_body(compiler, this->bodies[i]);
    compiler.end_scope();

    compiler.create_cond_branch(std::get<llvm::Value *>(test_is_true), merge_branch, next_branch);
    incoming_values.push_back({current_return_value, branch});
    branch = next_branch;
  }

  // else branch
  compiler.set_insert_point(branch);
  compiler.start_scope();
  llvm::Value *else_return_value = compile_branch_body(compiler, this->bodies.back());
  compiler.end_scope();
  incoming_values.push_back({else_return_value, branch});

  compiler.create_branch(merge_branch);
  compiler.set_insert_point(merge_branch);

  return compiler.create_phi(else_return_value->getType(), incoming_values);
}

std::unique_ptr<NormalExpression> NormalExpression::parse(Lexer &lexer) {
  auto expression = std::make_unique<NormalExpression>();
  expression->start_parsing(lexer, "<normal-expression>");

  // Keep parsing and tests until we have no longer ignored an `or` token
  while (true) {
    expression->and_tests.push_back(AndTest::parse(lexer));

    if (bool ignored_or = lexer.try_skip({Token::Type::OR}); !ignored_or)
      break;
  }

  expression->finish_parsing(lexer, "</normal-expression>");
  return expression;
}

llvm::Value *NormalExpression::compile(Compiler &compiler) const {
  llvm::Value *result = this->and_tests.front()->compile(compiler);

  for (size_t i = 1; i < and_tests.size(); ++i) {
    llvm::Value *rhs = this->and_tests[i]->compile(compiler);
    std::variant<llvm::Value *, BinaryOperatorError> operation = compiler.create_or(result, rhs);
    if (std::holds_alternative<llvm::Value *>(operation))
      result = std::get<llvm::Value *>(operation);
    else
      this->compiler_error(std::get<BinaryOperatorError>(operation));
  }

  return result;
}

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  auto expression = std::make_unique<FunctionExpression>();
  expression->start_parsing(lexer, "<function-expression>");

  expression->function = FunctionConstructor::parse(lexer);

  expression->finish_parsing(lexer, "</function-expression>");
  return expression;
}

llvm::Value *FunctionExpression::compile(Compiler &compiler) const {
  this->function->name = "__anonymous_function";
  return this->function->compile(compiler);
}

} // namespace Kebab::Parser
