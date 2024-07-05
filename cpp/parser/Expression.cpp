#include <cassert>
#include <memory>
#include <optional>
#include <vector>

#include "lexer/Token.hpp"
#include "parser/AndTest.hpp"
#include "parser/Constructor.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<Expression> Expression::parse(Lexer &lexer) {
  start_parsing("expression");
  std::unique_ptr<Expression> expression;

  switch (lexer.cur_token.type) {
  case Token::Type::IF:
    expression = CondExpression::parse(lexer);
    break;

  case Token::Type::INT_LITERAL:
  case Token::Type::FLOAT_LITERAL:
  case Token::Type::CHAR_LITERAL:
  case Token::Type::STRING_LITERAL:
  case Token::Type::NAME:
  case Token::Type::PLUS:
  case Token::Type::MINUS:
  case Token::Type::MULT:
  case Token::Type::DIV:
  case Token::Type::NOT:
  case Token::Type::TRUE:
  case Token::Type::FALSE:
  case Token::Type::LPAREN:
  case Token::Type::LBRACKET:
    expression = NormalExpression::parse(lexer);
    break;

  case Token::Type::FN:
    expression = FunctionExpression::parse(lexer);
    break;

  default:
    error(std::string("illegal syntax ") + lexer.cur_token.to_string());
  }

  end_parsing("expression");
  return expression;
}

void CondExpression::parse_test_body(Lexer &lexer) {
  start_parsing("cond-test-body");

  std::vector<std::unique_ptr<Statement>> body_statements;
  while (true) {
    std::optional<std::unique_ptr<Statement>> statement =
        Statement::try_parse_expression_statement(lexer);

    // If we managed to parse an expression statement that should be the final expression in the
    // cond expression's body - so we break. Otherwise keep parsing statements until we parse an
    // expression statement
    if (statement != std::nullopt) {
      body_statements.push_back(std::move(statement.value()));
      break;
    } else {
      body_statements.push_back(Statement::parse(lexer));
    }
  }

  end_parsing("cond-test-body");
}

void CondExpression::parse_if(Lexer &lexer) {
  start_parsing("cond-if");

  skip(lexer, Token::Type::IF);
  this->tests.push_back(Expression::parse(lexer));
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  end_parsing("cond-if");
}

void CondExpression::parse_elif(Lexer &lexer) {
  start_parsing("cond-elif");

  skip(lexer, Token::Type::ELIF);
  this->tests.push_back(Expression::parse(lexer));
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  end_parsing("cond-elif");
}

void CondExpression::parse_elifs(Lexer &lexer) {
  start_parsing("cond-elifs");

  while (lexer.cur_token.type == Token::Type::ELIF)
    this->parse_elif(lexer);

  end_parsing("cond-elifs");
}

void CondExpression::parse_else(Lexer &lexer) {
  start_parsing("cond-else");

  skip(lexer, Token::Type::ELSE);
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  end_parsing("cond-else");
}

std::unique_ptr<CondExpression> CondExpression::parse(Lexer &lexer) {
  start_parsing("cond-expression");
  std::unique_ptr<CondExpression> expression = std::make_unique<CondExpression>();

  expression->parse_if(lexer);
  expression->parse_elifs(lexer);
  expression->parse_else(lexer);

  end_parsing("cond-expression");
  return expression;
}

llvm::Value *CondExpression::compile(Compiler::Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function CondExpression::compile");
}

std::unique_ptr<NormalExpression> NormalExpression::parse(Lexer &lexer) {
  start_parsing("normal-expression");
  std::unique_ptr<NormalExpression> expression = std::make_unique<NormalExpression>();

  // Keep parsing and tests until we have no longer ignored an `or` token
  while (true) {
    expression->and_tests.push_back(AndTest::parse(lexer));

    bool ignored_or = ignore(lexer, Token::Type::OR);
    if (!ignored_or)
      break;
  }

  end_parsing("normal-expression");
  return expression;
}

llvm::Value *NormalExpression::compile(Compiler::Compiler &compiler) const {
  // for (std::unique_ptr<AndTest> const &and_test : this->and_tests)
  return this->and_tests.at(0)->compile(compiler);
}

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  start_parsing("function-expression");
  std::unique_ptr<FunctionExpression> expression = std::make_unique<FunctionExpression>();

  expression->function = FunctionConstructor::parse(lexer);

  end_parsing("function-expression");
  return expression;
}

llvm::Value *FunctionExpression::compile(Compiler::Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionExpression::compile");
}

} // namespace Parser
} // namespace Kebab
