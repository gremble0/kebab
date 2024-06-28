#include "Expression.hpp"
#include "parser/AndTest.hpp"

namespace Kebab {
namespace Parser {

Expression *Expression::parse(Lexer &lexer) {
  start_parsing("expression");
  Expression *expression;

  switch (lexer.cur_token.kind) {
  case Token::Kind::IF:
    expression = CondExpression::parse(lexer);
    break;

  case Token::Kind::NAME:
  case Token::Kind::CHAR_LITERAL:
  case Token::Kind::STRING_LITERAL:
  case Token::Kind::INT_LITERAL:
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

CondExpression *CondExpression::parse(Lexer &lexer) {
  start_parsing("cond-expression");

  // TODO:

  end_parsing("cond-expression");
  return nullptr;
}

NormalExpression *NormalExpression::parse(Lexer &lexer) {
  start_parsing("normal-expression");
  NormalExpression *expression = new NormalExpression();

  // Keep parsing and tests until we have no longer ignored an `or` token
  bool ignored_or;
  do {
    expression->and_tests.push_back(AndTest::parse(lexer));

    ignored_or = ignore(lexer, Token::Kind::OR);
  } while (!ignored_or);

  end_parsing("normal-expression");
  return expression;
}

FunctionExpression *FunctionExpression::parse(Lexer &lexer) {
  start_parsing("function-expression");

  // TODO:

  end_parsing("function-expression");
  return nullptr;
}

} // namespace Parser
} // namespace Kebab
