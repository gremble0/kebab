#include "Expression.hpp"
#include "parser/AndTest.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<Expression> Expression::parse(Lexer &lexer) {
  start_parsing("expression");
  std::unique_ptr<Expression> expression;

  switch (lexer.cur_token.kind) {
  case Token::Kind::IF:
    expression = CondExpression::parse(lexer);
    break;

  case Token::Kind::INT_LITERAL:
  case Token::Kind::FLOAT_LITERAL:
  case Token::Kind::CHAR_LITERAL:
  case Token::Kind::STRING_LITERAL:
  case Token::Kind::NAME:
  case Token::Kind::PLUS:
  case Token::Kind::MINUS:
  case Token::Kind::MULT:
  case Token::Kind::DIV:
  case Token::Kind::NOT:
  case Token::Kind::TRUE:
  case Token::Kind::FALSE:
  case Token::Kind::LPAREN:
  case Token::Kind::LBRACKET:
    expression = NormalExpression::parse(lexer);
    break;

  case Token::Kind::FN:
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

  // TODO:

  end_parsing("cond-expression");
  return nullptr;
}

std::unique_ptr<NormalExpression> NormalExpression::parse(Lexer &lexer) {
  start_parsing("normal-expression");
  std::unique_ptr<NormalExpression> expression = std::make_unique<NormalExpression>();

  // Keep parsing and tests until we have no longer ignored an `or` token
  while (true) {
    expression->and_tests.push_back(AndTest::parse(lexer));

    bool ignored_or = ignore(lexer, Token::Kind::OR);
    if (!ignored_or)
      break;
  }

  end_parsing("normal-expression");
  return expression;
}

std::unique_ptr<FunctionExpression> FunctionExpression::parse(Lexer &lexer) {
  start_parsing("function-expression");

  // TODO:

  end_parsing("function-expression");
  return nullptr;
}

} // namespace Parser
} // namespace Kebab
