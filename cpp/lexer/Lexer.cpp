#include <iostream>

#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(std::string path) : path(path) {}

void Lexer::advance() {
  Token a(TokenKind::EQ);
  std::cout << a.to_string() << std::endl;
}
