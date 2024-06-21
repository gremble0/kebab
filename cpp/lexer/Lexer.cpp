#include <iostream>
#include <ostream>
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
  if (stream.eof()) {
    this->cur_token = Token(TokenKind::TOKEN_EOF);
    return;
  }

  if (this->line_pos >= this->line.length())
    this->next_line();

  switch (this->line[this->line_pos]) {
  // Comment start means move on to next line
  case '\n':
  case ';':
    this->next_line();
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
    break;
  }

  ++this->line_pos;
  std::cout << cur_token->to_string() << std::endl;
}
