#include <cassert>
#include <memory>
#include <optional>
#include <vector>

#include "lexer/Token.hpp"
#include "logging/Logger.hpp"
#include "parser/AndTest.hpp"
#include "parser/Constructor.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/BasicBlock.h"

namespace Kebab {
namespace Parser {

std::unique_ptr<Expression> Expression::parse(Lexer &lexer) {
  std::unique_ptr<Expression> expression;

  switch (lexer.peek()->type) {
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
    parser_error(std::string("unexpected token '") + lexer.peek()->to_string_short() + '\'', lexer);
  }

  return expression;
}

void CondExpression::parse_test_body(Lexer &lexer) {
  Logger::log_with_indent("<cond-test-body>");

  std::vector<std::unique_ptr<Statement>> body_statements;
  while (true) {
    std::optional<std::unique_ptr<Statement>> statement = Statement::try_parse_statement(lexer);
    if (statement == std::nullopt)
      break;
    else
      body_statements.push_back(std::move(statement.value()));
  }

  if (!body_statements.back()->is_expression())
    this->parser_error("every branch in a cond expression must return a value", lexer);

  Logger::log_with_dedent("</cond-test-body>");
}

// TODO: maybe branches should be a class
// TODO: maybe class
void CondExpression::parse_if(Lexer &lexer) {
  Logger::log_with_indent("<cond-if>");

  lexer.skip({Token::Type::IF});
  this->tests.push_back(Expression::parse(lexer));
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("</cond-if>");
}

// TODO: maybe class
void CondExpression::parse_elif(Lexer &lexer) {
  Logger::log_with_indent("<cond-elif>");

  lexer.skip({Token::Type::ELIF});
  this->tests.push_back(Expression::parse(lexer));
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("<cond-elif/>");
}

// TODO: maybe class
void CondExpression::parse_elifs(Lexer &lexer) {
  Logger::log_with_indent("<cond-elifs>");

  while (lexer.peek()->type == Token::Type::ELIF)
    this->parse_elif(lexer);

  Logger::log_with_dedent("</cond-elifs>");
}

// TODO: maybe class
void CondExpression::parse_else(Lexer &lexer) {
  Logger::log_with_indent("<cond-else>");

  lexer.skip({Token::Type::ELSE});
  lexer.skip({Token::Type::FAT_RARROW});
  this->parse_test_body(lexer);

  Logger::log_with_dedent("</cond-else>");
}

std::unique_ptr<CondExpression> CondExpression::parse(Lexer &lexer) {
  std::unique_ptr<CondExpression> expression = std::make_unique<CondExpression>();
  expression->start_parsing(lexer, "<cond-expression>");

  expression->parse_if(lexer);
  expression->parse_elifs(lexer);
  expression->parse_else(lexer);

  expression->finish_parsing(lexer, "</cond-expression>");
  return expression;
}

llvm::Value *CondExpression::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function CondExpression::compile");
}

std::unique_ptr<NormalExpression> NormalExpression::parse(Lexer &lexer) {
  std::unique_ptr<NormalExpression> expression = std::make_unique<NormalExpression>();
  expression->start_parsing(lexer, "<normal-expression>");

  // Keep parsing and tests until we have no longer ignored an `or` token
  while (true) {
    expression->and_tests.push_back(AndTest::parse(lexer));

    bool ignored_or = lexer.try_skip({Token::Type::OR});
    if (!ignored_or)
      break;
  }

  expression->finish_parsing(lexer, "</normal-expression>");
  return expression;
}

llvm::Value *NormalExpression::compile(Compiler &compiler) const {
  // for (std::unique_ptr<AndTest> const &and_test : this->and_tests)
  return this->and_tests.at(0)->compile(compiler);
}

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  std::unique_ptr<FunctionExpression> expression = std::make_unique<FunctionExpression>();
  expression->start_parsing(lexer, "<function-expression>");

  expression->function = FunctionConstructor::parse(lexer);

  expression->finish_parsing(lexer, "</function-expression>");
  return expression;
}

llvm::Value *FunctionExpression::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionExpression::compile");
}

} // namespace Parser
} // namespace Kebab
