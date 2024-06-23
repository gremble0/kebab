#include <cstdint>
#include <string>

#include "Token.hpp"

std::string Token::to_string() const {
  switch (this->kind) {
  case Token::Kind::DEF:
    return "def";
  case Token::Kind::SET:
    return "set";
  case Token::Kind::MUT:
    return "mut";
  case Token::Kind::NIL:
    return "nil";
  case Token::Kind::IF:
    return "if";
  case Token::Kind::ELIF:
    return "elif";
  case Token::Kind::ELSE:
    return "else";
  case Token::Kind::FN:
    return "fn";

    // Booleans
  case Token::Kind::TRUE:
    return "true";
  case Token::Kind::FALSE:
    return "false";

    // Syntax
  case Token::Kind::COLON:
    return ":";
  case Token::Kind::EQUALS:
    return "=";
  case Token::Kind::COMMA:
    return ",";
  case Token::Kind::LPAREN:
    return "(";
  case Token::Kind::RPAREN:
    return ")";
  case Token::Kind::LBRACKET:
    return "[";
  case Token::Kind::RBRACKET:
    return "]";
  case Token::Kind::FAT_RARROW:
    return "=>";

    // Operators
  case Token::Kind::PLUS:
    return "+";
  case Token::Kind::MINUS:
    return "-";
  case Token::Kind::MULT:
    return "*";
  case Token::Kind::DIV:
    return "/";
  case Token::Kind::NOT:
    return "~";

    // Comparisons
  case Token::Kind::LT:
    return "<";
  case Token::Kind::LE:
    return "<=";
  case Token::Kind::EQ:
    return "==";
  case Token::Kind::NEQ:
    return "~=";
  case Token::Kind::GT:
    return ">";
  case Token::Kind::GE:
    return ">=";

    // Values
  case Token::Kind::INT_LITERAL:
    return std::to_string(std::get<int64_t>(this->value));
  case Token::Kind::FLOAT_LITERAL:
    return std::to_string(std::get<float_t>(this->value));
  case Token::Kind::CHAR_LITERAL:
    return std::to_string(std::get<uint8_t>(this->value));
  case Token::Kind::STRING_LITERAL:
    return '"' + std::get<std::string>(this->value) + '"';
  case Token::Kind::NAME:
    return std::get<std::string>(this->value);

    // Special
  case Token::Kind::END_OF_FILE:
    return "<end of file>";
  case Token::Kind::ILLEGAL:
    return "<illegal token>";
  }

  return "Unknown token";
}
