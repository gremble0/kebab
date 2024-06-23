#include "Token.hpp"

std::string TokenSimple::to_string() const {
  switch (this->kind_) {
  case TOKEN_DEF:
    return "def";
  case TOKEN_SET:
    return "set";
  case TOKEN_MUT:
    return "mut";
  case TOKEN_NIL:
    return "nil";
  case TOKEN_IF:
    return "if";
  case TOKEN_ELIF:
    return "elif";
  case TOKEN_ELSE:
    return "else";
  case TOKEN_FN:
    return "fn";
  case TOKEN_TRUE:
    return "true";
  case TOKEN_FALSE:
    return "false";
  case TOKEN_COLON:
    return ":";
  case TOKEN_EQUALS:
    return "=";
  case TOKEN_COMMA:
    return ",";
  case TOKEN_LPAREN:
    return "(";
  case TOKEN_RPAREN:
    return ")";
  case TOKEN_LBRACKET:
    return "[";
  case TOKEN_RBRACKET:
    return "]";
  case TOKEN_FAT_RARROW:
    return "=>";
  case TOKEN_PLUS:
    return "+";
  case TOKEN_MINUS:
    return "-";
  case TOKEN_MULT:
    return "*";
  case TOKEN_DIV:
    return "/";
  case TOKEN_NOT:
    return "~";
  case TOKEN_LT:
    return "<";
  case TOKEN_LE:
    return "<=";
  case TOKEN_EQ:
    return "==";
  case TOKEN_NEQ:
    return "~=";
  case TOKEN_GT:
    return ">";
  case TOKEN_GE:
    return ">=";
  case TOKEN_EOF:
    return "<eof>";
  case TOKEN_ILLEGAL:
    return "<illegal>";

  case TOKEN_INT_LITERAL:
  case TOKEN_FLOAT_LITERAL:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_NAME:
    assert(false);
  }

  return "unreachable";
}
