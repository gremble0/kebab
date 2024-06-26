#include "Expression.hpp"

Expression *Expression::parse(Lexer &lexer) {
  log_node_start("expression");
  Expression *expression;

  switch (lexer.cur_token.kind) {
  case Token::Kind::IF:
    expression = CondExpression::parse(lexer);
    expression->type = Expression::Type::COND;
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
    expression->type = Expression::Type::NORMAL;
    break;

  case Token::Kind::FN:
    expression = FunctionExpression::parse(lexer);
    expression->type = Expression::Type::FUNCTION;
    break;

  default:
    error("illegal syntax");
  }

  log_node_end("expression");
  return nullptr;
}
