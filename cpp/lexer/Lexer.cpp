#include <iostream>
#include <ostream>
#include <string>

#include "Lexer.hpp"

void Lexer::next_line() {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

void Lexer::advance() {
  if (this->line_pos >= this->line.length())
    this->next_line();

  if (stream.eof()) {
    this->cur_token = Token(TokenKind::TOKEN_EOF);
    return;
  }

  switch (this->line[this->line_pos]) {
  case '=':
    this->cur_token = Token(TokenKind::TOKEN_EQUALS);
    break;
  default:
    this->cur_token = Token(TokenKind::TOKEN_ILLEGAL);
    break;
  }

  ++this->line_pos;
  std::cout << cur_token->to_string() << std::endl;
}
