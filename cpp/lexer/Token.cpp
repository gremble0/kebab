#include "Token.hpp"
#include <string>

std::string Token::to_string() const {
  switch (this->kind) {
  case TokenKind::TOKEN_DEF:
    return "def";
  case TokenKind::TOKEN_SET:
    return "set";
  case TokenKind::TOKEN_MUT:
    return "mut";
  case TokenKind::TOKEN_NIL:
    return "nil";
  case TokenKind::TOKEN_IF:
    return "if";
  case TokenKind::TOKEN_ELIF:
    return "elif";
  case TokenKind::TOKEN_ELSE:
    return "else";
  case TokenKind::TOKEEN_FN:
    return "fn";

    // Booleans
  case TokenKind::TOKEN_TRUE:
    return "true";
  case TokenKind::TOKEN_FALSE:
    return "false";

    // Syntax
  case TokenKind::TOKEN_COLON:
    return ":";
  case TokenKind::TOKEN_EQUALS:
    return "=";
  case TokenKind::TOKEN_COMMA:
    return ",";
  case TokenKind::TOKEN_LPAREN:
    return "(";
  case TokenKind::TOKEN_RPAREN:
    return ")";
  case TokenKind::TOKEN_LBRACKET:
    return "[";
  case TokenKind::TOKEN_RBRACKET:
    return "]";
  case TokenKind::TOKEN_FAT_RARROW:
    return "=>";

    // Operators
  case TokenKind::TOKEN_PLUS:
    return "+";
  case TokenKind::TOKEN_MINUS:
    return "-";
  case TokenKind::TOKEN_MULT:
    return "*";
  case TokenKind::TOKEN_DIV:
    return "/";
  case TokenKind::TOKEN_NOT:
    return "~";

    // Comparisons
  case TokenKind::TOKEN_LT:
    return "<";
  case TokenKind::TOKEN_LE:
    return "<=";
  case TokenKind::TOKEN_EQ:
    return "==";
  case TokenKind::TOKEN_NEQ:
    return "~=";
  case TokenKind::TOKEN_GT:
    return ">";
  case TokenKind::TOKEN_GE:
    return ">=";

    // Values
  case TokenKind::TOKEN_INT_LITERAL:
    return std::to_string(this->i);
  case TokenKind::TOKEN_FLOAT_LITERAL:
    return std::to_string(this->f);
  case TokenKind::TOKEN_CHAR_LITERAL:
    return std::to_string(this->c);
  case TokenKind::TOKEN_STRING_LITERAL:
    return '"' + this->s + '"';
  case TokenKind::TOKEN_NAME:
    return this->s;

    // Special
  case TokenKind::TOKEN_EOF:
    return "<end of file>";
  }

  return "Unknown token";
}

// Token::~Token() {
//   if (this->kind == TokenKind::TOKEN_STRING_LITERAL || this->kind == TokenKind::TOKEN_NAME)
//     ~;
// }
