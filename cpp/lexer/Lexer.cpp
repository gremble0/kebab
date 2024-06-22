#include <cassert>
#include <cctype>
#include <cstdint>
#include <string>

#include "Lexer.hpp"

// TODO: replace asserts with some better error handling

void Lexer::next_line(void) {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

char Lexer::peek(int offset) {
  // If out of bounds return nullbyte
  if (this->line_pos + offset >= this->line.length() || this->line_pos + offset < 0)
    return '\0';

  return this->line[this->line_pos + offset];
}

// Returned token is either an integer literal or a float literal
Token Lexer::read_number(void) {
  size_t start_pos = this->line_pos;
  bool has_seen_point = false; // whether we have seen a '.' - should only happen once

  while (true) {
    char peeked = this->peek(0);

    if (std::isalnum(peeked)) {
      ++this->line_pos;
      continue;
    } else if (peeked == '.') {
      ++this->line_pos;
      if (has_seen_point)
        assert(false); // This is an error - a number should not have two '.'s

      has_seen_point = true;
      continue;
    } else {
      break;
    }
  }

  if (has_seen_point)
    return Token(std::stof(&this->line[start_pos]));
  else
    return Token(static_cast<int64_t>(std::stoi(&this->line[start_pos])));
}

Token Lexer::read_char(void) {
  assert(this->line_pos + 3 < this->line.length()); // Verify there are enough chars in the line
  assert(this->peek(0) == '\'');                    // Verify there is an opening quote
  assert(this->peek(2) == '\'');                    // Verify there is clsoing quote

  Token c(this->peek(1));
  this->line_pos += 3; // 1 for opening quote, 1 for char inside quotes, 1 for closing quote
  return c;
}

Token Lexer::read_string(void) {
  // We need at least an opening and a closing double quote to be able to read a string
  assert(this->line_pos + 2 < this->line.length());
  assert(this->peek(0) == '"'); // Verify there is an opening quote

  size_t start_pos = ++this->line_pos; // Skip opening quote (start_pos is after this quote)

  while (true) {
    char peeked = this->peek(0);
    assert(peeked != '\0'); // Verify we have not unexpectedly reached the end of the line

    if (peeked == '\\')
      ++this->line_pos; // Skip the next char as it is escaped
    else if (peeked == '"')
      break;

    ++this->line_pos; // Skip current char
  }

  assert(this->peek(0) == '"');      // Verify there is a closing quote
  size_t end_pos = this->line_pos++; // Skip closing quote (end_pos is before this quote)

  return Token(TokenKind::TOKEN_STRING_LITERAL, this->line.substr(start_pos, end_pos - start_pos));
}

void Lexer::advance(void) {
  switch (this->peek(0)) {
  // Nullbyte means end of the string effectively indicating a newline
  case '\0':
  // Comment start means we ignore the rest of the line
  case ';':
    this->next_line();

    if (stream.eof()) {
      this->cur_token = Token(TokenKind::TOKEN_EOF);
      break;
    }

    this->advance();
    break;

  // Whitespace is simply ignored
  case '\t':
  case ' ':
    ++this->line_pos;
    this->advance();
    break;

  // Syntax
  case ':':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_COLON);
    break;
  case '=':
    // Token depends on the next character in the line
    switch (this->peek(1)) {
    case '>':
      this->line_pos += 2;
      this->cur_token = Token(TokenKind::TOKEN_FAT_RARROW);
      break;
    case '=':
      this->line_pos += 2;
      this->cur_token = Token(TokenKind::TOKEN_EQ);
      break;
    default:
      ++this->line_pos;
      this->cur_token = Token(TokenKind::TOKEN_EQUALS);
      break;
    }
    break;
  case ',':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_COMMA);
    break;
  case '(':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_LPAREN);
    break;
  case ')':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_RPAREN);
    break;
  case '[':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_LBRACKET);
    break;
  case ']':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_RBRACKET);
    break;

  // Operators
  case '+':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_PLUS);
    break;
  case '-':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_MINUS);
    break;
  case '*':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_MULT);
    break;
  case '~':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = Token(TokenKind::TOKEN_NEQ);
      break;
    }

    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_NOT);
    break;
  case '<':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = Token(TokenKind::TOKEN_LE);
      break;
    }

    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_LT);
    break;
  case '>':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = Token(TokenKind::TOKEN_GE);
      break;
    }

    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_GT);
    break;
  case '/':
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_DIV);
    break;

  // Literals
  case '"':
    this->cur_token = Lexer::read_string();
    break;

  case '\'':
    this->cur_token = Lexer::read_char();
    break;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    this->cur_token = Lexer::read_number();
    break;

  default:
    this->cur_token = Token(TokenKind::TOKEN_ILLEGAL);
    ++this->line_pos;
    break;
  }
}
