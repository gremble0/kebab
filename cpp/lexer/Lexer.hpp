#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <string>

#include "Token.hpp"

class Lexer {
private:
  std::string path;
  std::string line;
  std::ifstream stream;
  size_t line_number;
  size_t line_pos;

  void next_line();
  char peek(int offset) const;
  [[noreturn]] void error(std::string message) const;

  Token read_number();
  Token read_char();
  Token read_string();
  Token read_word();

public:
  Token cur_token;

  Lexer(std::string path);
  void advance();
};

#endif
