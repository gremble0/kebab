#include <string>

#include "Lexer.hpp"

void Lexer::next_line(void) {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

char Lexer::peek(size_t offset) {
  // If out of bounds return nullbyte
  if (offset >= this->line.length())
    return '\0';

  return this->line[this->line_pos + offset];
}

void Lexer::advance(void) {
  switch (this->line[this->line_pos]) {
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

  default:
    this->cur_token = Token(TokenKind::TOKEN_ILLEGAL);
    ++this->line_pos;
    break;
  }
}
