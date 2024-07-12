#include <cstdint>
#include <string>

#include "lexer/Token.hpp"

namespace Kebab {

std::string Token::to_string_short() const {
  switch (this->type) {
  case Type::INT_LITERAL:
    return std::to_string(std::get<int64_t>(this->value));

  case Type::FLOAT_LITERAL:
    return std::to_string(std::get<float_t>(this->value));

  case Type::CHAR_LITERAL:
    return std::to_string(std::get<uint8_t>(this->value));

  case Type::STRING_LITERAL:
    return std::get<std::string>(this->value);

  case Type::NAME:
    return std::get<std::string>(this->value);

  default:
    return Token::type_to_string(this->type);
  }
}

std::string Token::to_string_verbose() const {
  std::string out = "<token ";

  switch (this->type) {
  case Type::INT_LITERAL:
    out += "int-literal: \"" + std::to_string(std::get<int64_t>(this->value)) + '"';
    break;

  case Type::FLOAT_LITERAL:
    out += "float-literal: \"" + std::to_string(std::get<float_t>(this->value)) + '"';
    break;

  case Type::CHAR_LITERAL:
    out += "char-literal: '" + std::to_string(std::get<uint8_t>(this->value)) + '\'';
    break;

  case Type::STRING_LITERAL:
    out += "string-literal: \"" + std::get<std::string>(this->value) + '"';
    break;

  case Type::NAME:
    out += "name: \"" + std::get<std::string>(this->value) + '"';
    break;

  default:
    out += '"' + Token::type_to_string(this->type) + '"';
    break;
  }

  out += ' ' + this->span.to_string() + '>';
  return out;
}

std::string Token::type_to_string(Type type) {
  switch (type) {
  case Type::DEF:
    return "def";
  case Type::SET:
    return "set";
  case Type::MUT:
    return "mut";
  case Type::NIL:
    return "nil";
  case Type::IF:
    return "if";
  case Type::ELIF:
    return "elif";
  case Type::ELSE:
    return "else";
  case Type::FN:
    return "fn";
  case Type::LIST:
    return "list";
  case Type::TRUE:
    return "true";
  case Type::FALSE:
    return "false";
  case Type::AND:
    return "and";
  case Type::OR:
    return "or";
  case Type::COLON:
    return ":";
  case Type::EQUALS:
    return "=";
  case Type::COMMA:
    return ",";
  case Type::LPAREN:
    return "(";
  case Type::RPAREN:
    return ")";
  case Type::LBRACKET:
    return "[";
  case Type::RBRACKET:
    return "]";
  case Type::FAT_RARROW:
    return "=>";
  case Type::PLUS:
    return "+";
  case Type::MINUS:
    return "-";
  case Type::MULT:
    return "*";
  case Type::DIV:
    return "/";
  case Type::NOT:
    return "~";
  case Type::LT:
    return "<";
  case Type::LE:
    return "<=";
  case Type::EQ:
    return "==";
  case Type::NEQ:
    return "~=";
  case Type::GT:
    return ">";
  case Type::GE:
    return ">=";
  case Type::INT_LITERAL:
    return "<int-literal>";
  case Type::FLOAT_LITERAL:
    return "<float-literal>";
  case Type::CHAR_LITERAL:
    return "<char-literal>";
  case Type::STRING_LITERAL:
    return "<string-literal>";
  case Type::NAME:
    return "<name>";
  case Type::END_OF_FILE:
    return "<end of file>";
  }
}

} // namespace Kebab
