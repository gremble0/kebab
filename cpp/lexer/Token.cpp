#include <cstdint>
#include <string>

#include "Token.hpp"

namespace Kebab {

Token::Token(Span span, std::string word) : span(span) {
  assert(word.find(' ') == std::string::npos);

  if (word.compare("def") == 0)
    this->type = DEF;
  else if (word.compare("set") == 0)
    this->type = SET;
  else if (word.compare("mut") == 0)
    this->type = MUT;
  else if (word.compare("if") == 0)
    this->type = IF;
  else if (word.compare("elif") == 0)
    this->type = ELIF;
  else if (word.compare("else") == 0)
    this->type = ELSE;
  else if (word.compare("fn") == 0)
    this->type = FN;
  else if (word.compare("list") == 0)
    this->type = LIST;
  else if (word.compare("true") == 0)
    this->type = TRUE;
  else if (word.compare("false") == 0)
    this->type = FALSE;
  else if (word.compare("and") == 0)
    this->type = AND;
  else if (word.compare("or") == 0)
    this->type = OR;
  else {
    this->type = NAME;
    this->value = word; // This should maybe be std::move ?
  }
}

std::string Token::to_string() const {
  switch (this->type) {
  case Token::Type::INT_LITERAL:
    return std::to_string(std::get<int64_t>(this->value));

  case Token::Type::FLOAT_LITERAL:
    return std::to_string(std::get<float_t>(this->value));

  case Token::Type::CHAR_LITERAL:
    return std::to_string(std::get<uint8_t>(this->value));

  case Token::Type::STRING_LITERAL:
    return std::get<std::string>(this->value);

  case Token::Type::NAME:
    return std::get<std::string>(this->value);

  default:
    return Token::type_to_string(this->type);
  }
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
