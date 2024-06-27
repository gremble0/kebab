#include "Expression.hpp"
#include "parser/AndTest.hpp"

Expression *Expression::parse(Lexer &lexer) {
  log_node_start("expression");
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

  log_node_end("expression");
  return expression;
}

CondExpression *CondExpression::parse(Lexer &lexer) {
  log_node_start("cond-expression");

  // TODO:

  log_node_end("cond-expression");
  return nullptr;
}

NormalExpression *NormalExpression::parse(Lexer &lexer) {
  log_node_start("normal-expression");
  NormalExpression *expression = new NormalExpression();

  // Keep parsing and tests until we have no longer ignored an `or` token
  bool ignored_or;
  do {
    expression->and_tests.push_back(AndTest::parse(lexer));

    ignored_or = ignore(lexer, Token::Kind::OR);
  } while (!ignored_or);

  log_node_end("normal-expression");
  return expression;
}

FunctionExpression *FunctionExpression::parse(Lexer &lexer) {
  log_node_start("function-expression");

  // TODO:

  log_node_end("function-expression");
  return nullptr;
}
