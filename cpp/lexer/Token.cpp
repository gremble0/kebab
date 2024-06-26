#include <cstdint>
#include <string>

#include "Token.hpp"

Token::Token(Span span, std::string word) : span(span) {
  assert(word.find(' ') == std::string::npos);

  if (word.compare("def") == 0)
    this->kind = DEF;
  else if (word.compare("set") == 0)
    this->kind = SET;
  else if (word.compare("mut") == 0)
    this->kind = MUT;
  else if (word.compare("if") == 0)
    this->kind = IF;
  else if (word.compare("elif") == 0)
    this->kind = ELIF;
  else if (word.compare("else") == 0)
    this->kind = ELSE;
  else if (word.compare("fn") == 0)
    this->kind = FN;
  else if (word.compare("true") == 0)
    this->kind = TRUE;
  else if (word.compare("false") == 0)
    this->kind = FALSE;
  else if (word.compare("and") == 0)
    this->kind = AND;
  else if (word.compare("or") == 0)
    this->kind = OR;
  else {
    this->kind = NAME;
    this->value = word;
  }
}

std::string Token::to_string() const {
  std::string out;

  switch (this->kind) {
  case Token::Kind::DEF:
    out = "<def>";
    break;
  case Token::Kind::SET:
    out = "<set>";
    break;
  case Token::Kind::MUT:
    out = "<mut>";
    break;
  case Token::Kind::NIL:
    out = "<nil>";
    break;
  case Token::Kind::IF:
    out = "<if>";
    break;
  case Token::Kind::ELIF:
    out = "<elif>";
    break;
  case Token::Kind::ELSE:
    out = "<else>";
    break;
  case Token::Kind::FN:
    out = "<fn>";
    break;

    // Booleans
  case Token::Kind::TRUE:
    out = "<true>";
    break;
  case Token::Kind::FALSE:
    out = "<false>";
    break;

  case Token::Kind::AND:
    out = "<and>";
    break;
  case Token::Kind::OR:
    out = "<or>";
    break;

  // Syntax
  case Token::Kind::COLON:
    out = "<colon>";
    break;
  case Token::Kind::EQUALS:
    out = "<equals>";
    break;
  case Token::Kind::COMMA:
    out = "<comma>";
    break;
  case Token::Kind::LPAREN:
    out = "<lparen>";
    break;
  case Token::Kind::RPAREN:
    out = "<rparen>";
    break;
  case Token::Kind::LBRACKET:
    out = "<lbracket>";
    break;
  case Token::Kind::RBRACKET:
    out = "<rbracket>";
    break;
  case Token::Kind::FAT_RARROW:
    out = "<fat-rarrow>";
    break;

  // Operators
  case Token::Kind::PLUS:
    out = "<plus>";
    break;
  case Token::Kind::MINUS:
    out = "<minus>";
    break;
  case Token::Kind::MULT:
    out = "<mult>";
    break;
  case Token::Kind::DIV:
    out = "<div>";
    break;
  case Token::Kind::NOT:
    out = "<not>";
    break;

  // Comparisons
  case Token::Kind::LT:
    out = "<lt>";
    break;
  case Token::Kind::LE:
    out = "<le>";
    break;
  case Token::Kind::EQ:
    out = "<eq>";
    break;
  case Token::Kind::NEQ:
    out = "<neq>";
    break;
  case Token::Kind::GT:
    out = "<gt>";
    break;
  case Token::Kind::GE:
    out = "<ge>";
    break;

  // Values
  case Token::Kind::INT_LITERAL:
    out = "<int-literal=" + std::to_string(std::get<int64_t>(this->value)) + '>';
    break;
  case Token::Kind::FLOAT_LITERAL:
    out = "<float-literal=" + std::to_string(std::get<float_t>(this->value)) + '>';
    break;
  case Token::Kind::CHAR_LITERAL:
    out = "<char-literal='" + std::to_string(std::get<uint8_t>(this->value)) + "'>";
    break;
  case Token::Kind::STRING_LITERAL:
    out = "<string-literal=\"" + std::get<std::string>(this->value) + "\">";
    break;
  case Token::Kind::NAME:
    out = "<name=\"" + std::get<std::string>(this->value) + "\">";
    break;

  // Special
  case Token::Kind::END_OF_FILE:
    out = "<end of file>";
    break;
  case Token::Kind::ILLEGAL:
    out = "<illegal token>";
    break;
  }

  return out + ' ' + this->span.to_string();
}
