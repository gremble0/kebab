#include <cctype>
#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "logging/Logger.hpp"

namespace Kebab {

Lexer::Lexer(const std::string &path) : path(path), stream(path), token() {
  if (!stream.is_open())
    this->error("could not open file " + path);

  this->next_line();
  this->advance();
}

// TODO: dont label as lexer-error since can be from parser, maybe make separate error handler class
[[noreturn]] void Lexer::error(const std::string &message) const {
  std::string pretty_position = this->pretty_position();
  std::string labeled_message = "lexer-error: " + message;

  std::cerr << pretty_position + labeled_message << std::endl;

  exit(1);
}

void Lexer::next_line() {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

std::string Lexer::pretty_position() const {
  std::string coordinates = std::format("{}:{}:{}\n", this->path, std::to_string(this->line_number),
                                        std::to_string(this->line_pos));
  std::string line_with_newline = this->line + '\n';
  // exclude leading spaces if line_pos is 0 to avoid underflow
  std::string line_cursor =
      this->line_pos > 0 ? std::string(this->line_pos - 1, ' ') + "^\n" : "^\n";

  return coordinates + line_with_newline + line_cursor;
}

uint8_t Lexer::peek(int offset) const {
  bool too_big = this->line_pos + offset >= this->line.length();
  bool too_small = (int)this->line_pos + offset < 0;
  if (too_big || too_small)
    return '\0';

  return this->line[this->line_pos + offset];
}

uint8_t Lexer::read_maybe_escaped_char() {
  uint8_t peeked = this->peek(0);
  uint8_t output;

  if (peeked == '\\') {
    uint8_t escaped = this->peek(1);
    if (escaped == 'n')
      output = '\n';
    else
      output = escaped;
    this->line_pos += 2;
  } else {
    output = peeked;
    ++this->line_pos;
  }

  return output;
}

// Returned token is either an integer literal or a float literal
void Lexer::handle_number() {
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
      this->token = std::make_unique<Token>(Token::Type::FLOAT_LITERAL, span,
                                            std::stof(&this->line[start.col]));
    else
      this->token = std::make_unique<Token>(Token::Type::INT_LITERAL, span,
                                            std::stoi(&this->line[start.col]));
  } catch (std::out_of_range &e) {
    this->error("number out of range");
  }
}

void Lexer::handle_char() {
  if (bool cant_read_char = this->line_pos + 3 >= this->line.length())
    this->error("unterminated char literal");
  bool missing_opening_quote = this->peek(0) != '\'';

  Position start = this->position();

  ++this->line_pos; // opening quote
  uint8_t c = this->read_maybe_escaped_char();
  bool missing_closing_quote = this->peek(0) != '\'';
  ++this->line_pos; // closing quote

  if (missing_opening_quote || missing_closing_quote)
    this->error("malformed char literal");

  Span span(start, this->position());

  this->token = std::make_unique<Token>(Token::Type::CHAR_LITERAL, span, c);
}

void Lexer::handle_string() {
  bool cant_read_char = this->line_pos + 2 >= this->line.length();
  bool missing_opening_quote = this->peek(0) != '"';
  if (cant_read_char || missing_opening_quote)
    this->error("malformed string literal");

  Position start = this->position();
  std::ostringstream string_stream;
  ++this->line_pos; // skip opening quote

  while (true) {
    uint8_t read_char = this->read_maybe_escaped_char();
    if (read_char == '\0')
      this->error("unterminated string literal");
    else if (read_char == '"')
      break;
    else
      string_stream << read_char;
  }
  // closing quote is skiped by read_maybe_escaped_char method

  Position end = this->position();
  Span span(start, end);

  this->token = std::make_unique<Token>(Token::Type::STRING_LITERAL, span, string_stream.str());
}

void Lexer::handle_word() {
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

  this->token = std::make_unique<Token>(span, word);
}

void Lexer::handle_newline() {
  uint8_t peeked = this->peek(0);
  assert(peeked == '\0' || peeked == ';');

  this->next_line();

  if (stream.eof()) {
    this->token =
        std::make_unique<Token>(Token::Type::END_OF_FILE, Span(this->position(), this->position()));
    return;
  }

  this->advance();
}

void Lexer::handle_one_char_type(Token::Type type) {
  Position start = this->position();
  ++this->line_pos;
  this->token = std::make_unique<Token>(type, Span(start, this->position()));
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

  this->token = std::make_unique<Token>(type, Span(start, this->position()));
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

  this->token = std::make_unique<Token>(type, Span(start, this->position()));
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

  this->token = std::make_unique<Token>(type, Span(start, this->position()));
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

  this->token = std::make_unique<Token>(type, Span(start, this->position()));
}

template <typename T, Token::Type K> T Lexer::skip_value() {
  this->expect(K);
  T value = std::get<T>(this->peek()->value);
  this->advance();
  return value;
}

void Lexer::handle_div() { this->handle_one_char_type(Token::Type::DIV); }

int64_t Lexer::skip_int() { return skip_value<int64_t, Token::Type::INT_LITERAL>(); }

float_t Lexer::skip_float() { return skip_value<float_t, Token::Type::FLOAT_LITERAL>(); }

uint8_t Lexer::skip_char() { return skip_value<uint8_t, Token::Type::CHAR_LITERAL>(); }

std::string Lexer::skip_string() { return skip_value<std::string, Token::Type::STRING_LITERAL>(); }

std::string Lexer::skip_name() { return skip_value<std::string, Token::Type::NAME>(); }

void Lexer::expect(Token::Type expected) const {
  if (this->token->type != expected)
    this->error("unexpected token '" + this->token->to_string_short() + "' expected: '" +
                Token::type_to_string(expected) + '\'');
}

void Lexer::expect(Token::Type either, Token::Type or_) const {
  if (this->token->type != either && this->token->type != or_)
    this->error("unexpected token '" + this->token->to_string_short() + "' expected: '" +
                Token::type_to_string(either) + "' or '" + Token::type_to_string(or_) + '\'');
}

void Lexer::skip(Token::Type type) {
  this->expect(type);
  this->advance();
}

void Lexer::skip(Token::Type either, Token::Type or_) {
  this->expect(either, or_);
  this->advance();
}

bool Lexer::try_skip(Token::Type expected) {
  if (this->token->type == expected) {
    this->advance();
    return true;
  }
  return false;
}

void Lexer::advance() {
  switch (char peeked = this->peek(0)) {
  // Nullbyte means end of the string effectively indicating a newline
  // Comment start means we ignore the rest of the line
  case '\0':
  case ';':
    return this->handle_newline();

  case '\t':
  case ' ':
    return this->handle_whitespace();

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
    this->handle_string();
    break;
  case '\'':
    this->handle_char();
    break;

  // Either a keyword, a name, a number or an illegal token
  default:
    if (std::isalpha(peeked))
      this->handle_word();
    else if (std::isalnum(peeked))
      this->Lexer::handle_number();
    else
      this->error(std::string("illegal token '") + peeked + "'");

    break;
  }

  Logger::log(this->token->to_string_verbose());
}

} // namespace Kebab
