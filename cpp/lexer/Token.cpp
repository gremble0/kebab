#include <cstdint>
#include <string>

#include "Token.hpp"

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
  case TokenKind::TOKEN_FN:
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
    return std::to_string(std::get<int64_t>(this->value));
  case TokenKind::TOKEN_FLOAT_LITERAL:
    return std::to_string(std::get<float_t>(this->value));
  case TokenKind::TOKEN_CHAR_LITERAL:
    return std::to_string(std::get<uint8_t>(this->value));
  case TokenKind::TOKEN_STRING_LITERAL:
    return '"' + std::get<std::string>(this->value) + '"';
  case TokenKind::TOKEN_NAME:
    return std::get<std::string>(this->value);

    // Special
  case TokenKind::TOKEN_EOF:
    return "<end of file>";
  case TokenKind::TOKEN_ILLEGAL:
    return "<illegal token>";
  }

  return "Unknown token";
}
