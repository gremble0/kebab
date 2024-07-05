#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#include "Lexer.hpp"
#include "Token.hpp"

namespace Kebab {

Lexer::Lexer(std::string path)
    : path(path), stream(path), line_number(0), line_pos(0),
      cur_token(Token::Type::ILLEGAL, Span()) {
  if (!stream.is_open())
    this->error("could not open file " + path);

  this->next_line();
  this->advance();
}

[[noreturn]] void Lexer::error(std::string message) const {
  std::string where = this->path + ':' + std::to_string(this->line_number) + ':' +
                      std::to_string(this->line_pos) + '\n';
  std::string line = this->line + '\n';
  std::string line_cursor = std::string(this->line_pos, ' ') + "^\n";
  std::string full_message = "lexer-error: " + message + '\n';

  std::cerr << where + line + line_cursor + full_message;

  exit(1);
}

void Lexer::next_line() {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

Position Lexer::position() const { return Position(this->line_number, this->line_pos); }

uint8_t Lexer::peek(int offset) const {
  bool too_big = this->line_pos + offset >= this->line.length();
  bool too_small = (int)this->line_pos + offset < 0;
  if (too_big || too_small)
    return '\0';

  return this->line[this->line_pos + offset];
}

// Returned token is either an integer literal or a float literal
Token Lexer::read_number() {
  Position start = this->position();
  bool has_seen_point = false;

  while (true) {
    char peeked = this->peek(0);

    if (std::isalnum(peeked)) {
      ++this->line_pos;
    } else if (peeked == '.') {
      ++this->line_pos;
      if (has_seen_point)
        this->error("malformed float literal");

      has_seen_point = true;
    } else {
      break;
    }
  }

  Span span(start, this->position());

  try {
    if (has_seen_point)
      return Token(Token::Type::FLOAT_LITERAL, span, std::stof(&this->line[start.col]));
    else
      return Token(Token::Type::INT_LITERAL, span, std::stoi(&this->line[start.col]));
  } catch (std::out_of_range) {
    this->error("number out of range");
  }
}

Token Lexer::read_char() {
  bool cant_read_char = this->line_pos + 3 >= this->line.length();
  if (cant_read_char)
    this->error("unterminated char literal");
  bool missing_opening_quote = this->peek(0) != '\'';

  Position start = this->position();

  ++this->line_pos; // opening quote
  uint8_t c = this->read_maybe_escaped_char();
  ++this->line_pos; // closing quote

  bool missing_closing_quote = this->peek(0) != '\'';
  if (missing_opening_quote || missing_closing_quote)
    this->error("malformed char literal");

  Span span(start, this->position());

  return Token(Token::Type::CHAR_LITERAL, span, c);
}

uint8_t Lexer::read_maybe_escaped_char() {
  uint8_t peeked = this->peek(0);
  uint8_t output;

  switch (peeked) {
  case '\\': {
    uint8_t escaped = this->peek(1);
    switch (escaped) {
    case 'n':
      output = '\n';
      break;

    default:
      output = escaped;
      break;
    }
    this->line_pos += 2;
    break;
  }

  default:
    output = peeked;
    ++this->line_pos;
    break;
  }

  return output;
}

Token Lexer::read_string() {
  bool cant_read_char = this->line_pos + 2 >= this->line.length();
  bool missing_opening_quote = this->peek(0) != '"';
  if (cant_read_char || missing_opening_quote)
    this->error("malformed string literal");

  Position start = this->position();
  std::ostringstream stream;
  ++this->line_pos; // skip opening quote

  while (true) {
    uint8_t read_char = this->read_maybe_escaped_char();
    if (read_char == '\0')
      this->error("unterminated string literal");
    else if (read_char == '"')
      break;
    else
      stream << read_char;
  }
  // closing quote is skiped by read_maybe_escaped_char method

  Position end = this->position();
  Span span(start, end);

  return Token(Token::Type::STRING_LITERAL, span, stream.str());
}

Token Lexer::read_word() {
  auto is_kebab_case = [](uint8_t c) {
    return !std::isspace(c) && c != ',' && c != '(' && c != ')' && c != '[' && c != ']' &&
           c != '\0';
  };

  if (!std::isalpha(this->peek(0)))
    this->error("token should start with a letter");

  Position start = this->position();
  while (is_kebab_case(this->peek(0)))
    ++this->line_pos;
  Position end = this->position();
  Span span(start, end);

  std::string word = this->line.substr(start.col, end.col - start.col);

  return Token(span, word);
}

void Lexer::handle_newline() {
  uint8_t peeked = this->peek(0);
  assert(peeked == '\0' || peeked == ';');

  this->next_line();

  if (stream.eof()) {
    this->cur_token = Token(Token::Type::END_OF_FILE, Span(this->position(), this->position()));
    return;
  }

  this->advance();
}

void Lexer::handle_one_char_type(Token::Type type) {
  Position start = this->position();
  ++this->line_pos;
  this->cur_token = Token(type, Span(start, this->position()));
}

void Lexer::handle_whitespace() {
  ++this->line_pos;
  this->advance();
}

void Lexer::handle_colon() { this->handle_one_char_type(Token::Type::COLON); }

void Lexer::handle_equals() {
  Position start = this->position();
  Token::Type type;

  switch (this->peek(1)) {
  case '>':
    this->line_pos += 2;
    type = Token::Type::FAT_RARROW;
    break;

  case '=':
    this->line_pos += 2;
    type = Token::Type::EQ;
    break;

  default:
    ++this->line_pos;
    type = Token::Type::EQUALS;
    break;
  }

  this->cur_token = Token(type, Span(start, this->position()));
}

void Lexer::handle_comma() { this->handle_one_char_type(Token::Type::COMMA); }

void Lexer::handle_lparen() { this->handle_one_char_type(Token::Type::LPAREN); }

void Lexer::handle_rparen() { this->handle_one_char_type(Token::Type::RPAREN); }

void Lexer::handle_lbracket() { this->handle_one_char_type(Token::Type::LBRACKET); }

void Lexer::handle_rbracket() { this->handle_one_char_type(Token::Type::RBRACKET); }

void Lexer::handle_plus() { this->handle_one_char_type(Token::Type::PLUS); }

void Lexer::handle_minus() { this->handle_one_char_type(Token::Type::MINUS); }

void Lexer::handle_mult() { this->handle_one_char_type(Token::Type::MULT); }

void Lexer::handle_not() {
  Position start = this->position();
  Token::Type type;

  if (this->peek(1) == '=') {
    this->line_pos += 2;
    type = Token::Type::NEQ;
  } else {
    ++this->line_pos;
    type = Token::Type::NOT;
  }

  this->cur_token = Token(type, Span(start, this->position()));
}

void Lexer::handle_lt() {
  Position start = this->position();
  Token::Type type;

  if (this->peek(1) == '=') {
    this->line_pos += 2;
    type = Token::Type::LE;
  } else {
    ++this->line_pos;
    type = Token::Type::LT;
  }

  this->cur_token = Token(type, Span(start, this->position()));
}

void Lexer::handle_gt() {
  Position start = this->position();
  Token::Type type;

  if (this->peek(1) == '=') {
    this->line_pos += 2;
    type = Token::Type::GE;
  } else {
    ++this->line_pos;
    type = Token::Type::GT;
  }

  this->cur_token = Token(type, Span(start, this->position()));
}

void Lexer::handle_div() { this->handle_one_char_type(Token::Type::DIV); }

void Lexer::advance() {
  char peeked = this->peek(0);

  switch (peeked) {
  // Nullbyte means end of the string effectively indicating a newline
  // Comment start means we ignore the rest of the line
  case '\0':
  case ';':
    this->handle_newline();
    break;

  case '\t':
  case ' ':
    this->handle_whitespace();
    break;

  // Syntax
  case ':':
    this->handle_colon();
    break;
  case '=':
    this->handle_equals();
    break;
  case ',':
    this->handle_comma();
    break;
  case '(':
    this->handle_lparen();
    break;
  case ')':
    this->handle_rparen();
    break;
  case '[':
    this->handle_lbracket();
    break;
  case ']':
    this->handle_rbracket();
    break;

  // Operators
  case '+':
    this->handle_plus();
    break;
  case '-':
    this->handle_minus();
    break;
  case '*':
    this->handle_mult();
    break;
  case '~':
    this->handle_not();
    break;
  case '<':
    this->handle_lt();
    break;
  case '>':
    this->handle_gt();
    break;
  case '/':
    this->handle_div();
    break;

  // Literals
  case '"':
    this->cur_token = Lexer::read_string();
    break;
  case '\'':
    this->cur_token = Lexer::read_char();
    break;

  // Either a keyword, a name, a number or an illegal token
  default:
    if (std::isalpha(peeked))
      this->cur_token = Lexer::read_word();
    else if (std::isalnum(peeked))
      this->cur_token = Lexer::read_number();
    else
      this->error(std::string("illegal token '") + peeked + "'");

    break;
  }
}

} // namespace Kebab
