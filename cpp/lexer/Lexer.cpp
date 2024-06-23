#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(std::string path)
    : path(path), stream(path), line_number(0), line_pos(0), cur_token() {
  if (!stream.is_open())
    this->error("could not open file " + path);

  this->next_line();
}

[[noreturn]] void Lexer::error(std::string message) const {
  std::string where = this->path + ':' + std::to_string(this->line_number) + ':' +
                      std::to_string(this->line_pos) + '\n';
  std::string line = this->line + '\n';
  std::string line_cursor = std::string(this->line_pos - 1, ' ') + "^\n";
  std::string full_message = "lexer-error: " + message + '\n';

  std::cerr << where + line + line_cursor + full_message;

  exit(1);
}

void Lexer::next_line() {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

char Lexer::peek(int offset) const {
  bool too_big = this->line_pos + offset >= this->line.length();
  bool too_small = this->line_pos + offset < 0;
  if (too_big || too_small)
    return '\0';

  return this->line[this->line_pos + offset];
}

// Returned token is either an integer literal or a float literal
Token Lexer::read_number() {
  size_t start_pos = this->line_pos;
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

  if (has_seen_point)
    return Token(Token::Kind::FLOAT_LITERAL, std::stof(&this->line[start_pos]));
  else
    return Token(Token::Kind::INT_LITERAL, static_cast<int64_t>(std::stoi(&this->line[start_pos])));
}

Token Lexer::read_char() {
  bool cant_read_char = this->line_pos + 3 >= this->line.length();
  bool missing_opening_quote = this->peek(0) != '\'';
  bool missing_closing_quote = this->peek(2) != '\'';
  if (cant_read_char || missing_opening_quote || missing_closing_quote)
    this->error("malformed char literal");

  Token c(Token::Kind::CHAR_LITERAL, this->peek(1));
  this->line_pos += 3;
  return c;
}

Token Lexer::read_string() {
  bool cant_read_char = this->line_pos + 2 >= this->line.length();
  bool missing_opening_quote = this->peek(0) != '"';
  if (cant_read_char || missing_opening_quote)
    this->error("malformed string literal");

  size_t start_pos = ++this->line_pos; // Skip opening quote (start_pos is after this quote)

  while (true) {
    char peeked = this->peek(0);

    if (peeked == '\0')
      this->error("unterminated string literal");
    else if (peeked == '\\')
      ++this->line_pos;
    else if (peeked == '"')
      break;

    ++this->line_pos;
  }

  size_t end_pos = this->line_pos++; // Skip closing quote (end_pos is before this quote)

  return Token(Token::Kind::STRING_LITERAL, this->line.substr(start_pos, end_pos - start_pos));
}

Token Lexer::read_word() {
  auto is_kebab_case = [](char c) {
    return !std::isspace(c) && c != ',' && c != '(' && c != ')' && c != '[' && c != ']';
  };

  if (!std::isalpha(this->peek(0)))
    this->error("words should start with a letter");

  size_t start_pos = this->line_pos;
  while (is_kebab_case(this->peek(0)))
    ++this->line_pos;
  size_t end_pos = this->line_pos;

  std::string word = this->line.substr(start_pos, end_pos - start_pos);

  return Token(word);
}

void Lexer::handle_newline() {
  char peeked = this->peek(0);
  assert(peeked == '\0' || peeked == ';');

  this->next_line();

  if (stream.eof()) {
    this->cur_token = Token(Token::Kind::END_OF_FILE);
    return;
  }

  this->advance();
}

void Lexer::handle_whitespace() {
  ++this->line_pos;
  this->advance();
}

void Lexer::handle_colon() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::COLON);
}

void Lexer::handle_equals() {
  assert(this->peek(0) == '=');

  switch (this->peek(1)) {
  case '>':
    this->line_pos += 2;
    this->cur_token = Token(Token::Kind::FAT_RARROW);
    break;
  case '=':
    this->line_pos += 2;
    this->cur_token = Token(Token::Kind::EQ);
    break;
  default:
    ++this->line_pos;
    this->cur_token = Token(Token::Kind::EQUALS);
    break;
  }
}

void Lexer::handle_comma() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::COMMA);
}

void Lexer::handle_lparen() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::LPAREN);
}

void Lexer::handle_rparen() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::RPAREN);
}

void Lexer::handle_lbracket() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::LBRACKET);
}

void Lexer::handle_rbracket() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::RBRACKET);
}

void Lexer::handle_plus() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::PLUS);
}

void Lexer::handle_minus() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::MINUS);
}

void Lexer::handle_mult() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::MULT);
}

void Lexer::handle_not() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(Token::Kind::NEQ);
  } else {
    ++this->line_pos;
    this->cur_token = Token(Token::Kind::NOT);
  }
}

void Lexer::handle_lt() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(Token::Kind::LE);
  } else {
    ++this->line_pos;
    this->cur_token = Token(Token::Kind::LT);
  }
}

void Lexer::handle_gt() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(Token::Kind::GE);
  } else {
    ++this->line_pos;
    this->cur_token = Token(Token::Kind::GT);
  }
}

void Lexer::handle_div() {
  ++this->line_pos;
  this->cur_token = Token(Token::Kind::DIV);
}

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
    if (std::isalpha(peeked)) {
      this->cur_token = Lexer::read_word();
    } else if (std::isalnum(peeked)) {
      this->cur_token = Lexer::read_number();
    } else {
      this->cur_token = Token(Token::Kind::ILLEGAL);
      ++this->line_pos;
    }
    break;
  }
}
