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

namespace Kebab {
namespace Parser {

std::unique_ptr<Expression> Expression::parse(Lexer &lexer) {
  Logger::log_with_indent("<expression>");
  std::unique_ptr<Expression> expression;

  switch (lexer.cur_token->type) {
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
    error(std::string("unexpected token '") + lexer.cur_token->to_string_short() + '\'', lexer);
  }

  Logger::log_with_dedent("<expression/>");
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
    this->error("every branch in a cond expression must return a value", lexer);

  Logger::log_with_dedent("<cond-test-body/>");
}

void CondExpression::parse_if(Lexer &lexer) {
  Logger::log_with_indent("<cond-if>");

  skip(lexer, Token::Type::IF);
  this->tests.push_back(Expression::parse(lexer));
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  Logger::log_with_dedent("<cond-if/>");
}

void CondExpression::parse_elif(Lexer &lexer) {
  Logger::log_with_indent("<cond-elif>");

  skip(lexer, Token::Type::ELIF);
  this->tests.push_back(Expression::parse(lexer));
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  Logger::log_with_dedent("<cond-elif/>");
}

void CondExpression::parse_elifs(Lexer &lexer) {
  Logger::log_with_indent("<cond-elifs>");

  while (lexer.cur_token->type == Token::Type::ELIF)
    this->parse_elif(lexer);

  Logger::log_with_dedent("<cond-elifs/>");
}

void CondExpression::parse_else(Lexer &lexer) {
  Logger::log_with_indent("<cond-else>");

  skip(lexer, Token::Type::ELSE);
  skip(lexer, Token::Type::FAT_RARROW);
  this->parse_test_body(lexer);

  Logger::log_with_dedent("<cond-else/>");
}

std::unique_ptr<CondExpression> CondExpression::parse(Lexer &lexer) {
  Logger::log_with_indent("<cond-expression>");
  std::unique_ptr<CondExpression> expression = std::make_unique<CondExpression>();

  expression->parse_if(lexer);
  expression->parse_elifs(lexer);
  expression->parse_else(lexer);

  Logger::log_with_dedent("<cond-expression/>");
  return expression;
}

llvm::Value *CondExpression::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function CondExpression::compile");
}

std::unique_ptr<NormalExpression> NormalExpression::parse(Lexer &lexer) {
  Logger::log_with_indent("<normal-expression>");
  std::unique_ptr<NormalExpression> expression = std::make_unique<NormalExpression>();

  // Keep parsing and tests until we have no longer ignored an `or` token
  while (true) {
    expression->and_tests.push_back(AndTest::parse(lexer));

    bool ignored_or = ignore(lexer, Token::Type::OR);
    if (!ignored_or)
      break;
  }

  Logger::log_with_dedent("<normal-expression/>");
  return expression;
}

llvm::Value *NormalExpression::compile(Compiler &compiler) const {
  // for (std::unique_ptr<AndTest> const &and_test : this->and_tests)
  return this->and_tests.at(0)->compile(compiler);
}

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  Logger::log_with_indent("<function-expression>");
  std::unique_ptr<FunctionExpression> expression = std::make_unique<FunctionExpression>();

  expression->function = FunctionConstructor::parse(lexer);

  Logger::log_with_dedent("<function-expression/>");
  return expression;
}

llvm::Value *FunctionExpression::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionExpression::compile");
}

} // namespace Parser
} // namespace Kebab
