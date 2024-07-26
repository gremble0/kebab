#include <cmath>
#include <cstdint>
#include <format>
#include <string>

#include "lexer/Token.hpp"

namespace Kebab {

std::string Token::to_string_short() const {
  switch (this->type) {
    using enum Type;
  case INT_LITERAL:
    return std::to_string(std::get<int64_t>(this->value));

  case FLOAT_LITERAL:
    return std::to_string(std::get<double_t>(this->value));

  case CHAR_LITERAL:
    return std::format("'{}'", std::to_string(std::get<uint8_t>(this->value)));

  case STRING_LITERAL:
    return std::format("\"{}\"", std::get<std::string>(this->value));

  case NAME:
    return std::get<std::string>(this->value);

  default:
    return Token::type_to_string(this->type);
  }
}

std::string Token::to_string_verbose() const {
  std::string out = "<token ";

  switch (this->type) {
    using enum Type;
  case INT_LITERAL:
    out += std::format("int-literal: \"{}\"", std::to_string(std::get<int64_t>(this->value)));
    break;

  case FLOAT_LITERAL:
    out += std::format("float-literal: \"{}\"", std::to_string(std::get<double_t>(this->value)));
    break;

  case CHAR_LITERAL:
    out += std::format("char-literal: '{}'", std::to_string(std::get<uint8_t>(this->value)));
    break;

  case STRING_LITERAL:
    out += std::format("string-literal: \"{}\"", std::get<std::string>(this->value));
    break;

  case NAME:
    out += std::format("name: \"{}\"", std::get<std::string>(this->value));
    break;

  default:
    out += std::format("\"{}\"", Token::type_to_string(this->type));
    break;
  }

  out += std::format(" {}>", this->span.to_string());
  return out;
}

std::string Token::type_to_string(Type type) {
  switch (type) {
    using enum Type;
  case DEF:
    return "def";
  case SET:
    return "set";
  case MUT:
    return "mut";
  case NIL:
    return "nil";
  case IF:
    return "if";
  case ELIF:
    return "elif";
  case ELSE:
    return "else";
  case FN:
    return "fn";
  case LIST:
    return "list";
  case TRUE:
    return "true";
  case FALSE:
    return "false";
  case AND:
    return "and";
  case OR:
    return "or";
  case COLON:
    return ":";
  case EQUALS:
    return "=";
  case COMMA:
    return ",";
  case LPAREN:
    return "(";
  case RPAREN:
    return ")";
  case LBRACKET:
    return "[";
  case RBRACKET:
    return "]";
  case FAT_RARROW:
    return "=>";
  case PLUS:
    return "+";
  case MINUS:
    return "-";
  case MULT:
    return "*";
  case DIV:
    return "/";
  case NOT:
    return "~";
  case LT:
    return "<";
  case LE:
    return "<=";
  case EQ:
    return "==";
  case NEQ:
    return "~=";
  case GT:
    return ">";
  case GE:
    return ">=";
  case INT_LITERAL:
    return "<int-literal>";
  case FLOAT_LITERAL:
    return "<float-literal>";
  case CHAR_LITERAL:
    return "<char-literal>";
  case STRING_LITERAL:
    return "<string-literal>";
  case NAME:
    return "<name>";
  case END_OF_FILE:
    return "<end of file>";
  }
}

} // namespace Kebab
