#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <string>

#include "Token.hpp"

class Lexer {
private:
  std::string path;
  std::string line;
  std::ifstream stream;
  size_t line_number;
  size_t line_pos;

  void next_line(void);
  char peek(int offset);

  std::optional<Token> read_number(void);
  std::optional<Token> read_char(void);
  std::optional<Token> read_string(void);
  std::optional<Token> read_word(void);

public:
  std::optional<Token> cur_token;

  Lexer(std::string path);
  void advance(void);
};

#endif
