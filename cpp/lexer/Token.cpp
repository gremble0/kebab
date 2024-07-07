#include <cstdint>
#include <string>

#include "lexer/Token.hpp"

namespace Kebab {

Token::Type Token::determine_type(const std::string &word) {
  if (word == "def")
    return DEF;
  else if (word == "set")
    return SET;
  else if (word == "mut")
    return MUT;
  else if (word == "if")
    return IF;
  else if (word == "elif")
    return ELIF;
  else if (word == "else")
    return ELSE;
  else if (word == "fn")
    return FN;
  else if (word == "list")
    return LIST;
  else if (word == "true")
    return TRUE;
  else if (word == "false")
    return FALSE;
  else if (word == "and")
    return AND;
  else if (word == "or")
    return OR;
  else
    return NAME;
}

std::variant<uint8_t, int64_t, float_t, std::string>
Token::determine_value(Type type, const std::string &word) {
  if (type == NAME)
    return word;

  return {};
}

std::string Token::to_string() const {
  std::string out = "<token '";

  switch (this->type) {
  case Token::Type::INT_LITERAL:
    out += std::to_string(std::get<int64_t>(this->value));
    break;

  case Token::Type::FLOAT_LITERAL:
    out += std::to_string(std::get<float_t>(this->value));
    break;

  case Token::Type::CHAR_LITERAL:
    out += std::to_string(std::get<uint8_t>(this->value));
    break;

  case Token::Type::STRING_LITERAL:
    out += std::get<std::string>(this->value);
    break;

  case Token::Type::NAME:
    out += std::get<std::string>(this->value);
    break;

  default:
    out += Token::type_to_string(this->type);
    break;
  }

  out += "' " + this->span.to_string() + '>';
  return out;
}

std::string Token::type_to_string(Type type) {
  switch (type) {
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
  case ILLEGAL:
    return "<illegal token>";
  }
}

} // namespace Kebab
