#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <memory>
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

  std::unique_ptr<Token> read_number();
  std::unique_ptr<Token> read_char(); // TokenCharLiteral?
  std::unique_ptr<Token> read_string();
  std::unique_ptr<Token> read_word();

public:
  std::unique_ptr<Token> cur_token;

  Lexer(std::string path);
  void advance();
};

#endif
