#include "Expression.hpp"

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

  log_node_end("normal-expression");
  return nullptr;
}

FunctionExpression *FunctionExpression::parse(Lexer &lexer) {
  log_node_start("function-expression");

  // TODO:

  log_node_end("function-expression");
  return nullptr;
}
