#include <memory>

#include "Expression.hpp"
#include "parser/AndTest.hpp"
#include "parser/Constructor.hpp"
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
    error("illegal syntax");
  }

  end_parsing("expression");
  return expression;
}

std::unique_ptr<CondExpression> CondExpression::parse(Lexer &lexer) {
  start_parsing("cond-expression");
  std::unique_ptr<CondExpression> expression = std::make_unique<CondExpression>();

  // TODO:

  end_parsing("cond-expression");
  return expression;
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

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  start_parsing("function-expression");
  std::unique_ptr<FunctionExpression> expression = std::make_unique<FunctionExpression>();

  // TODO:

  end_parsing("function-expression");
  return expression;
}

} // namespace Parser
} // namespace Kebab
