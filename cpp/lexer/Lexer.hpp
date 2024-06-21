#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
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

  void next_line();

public:
  std::optional<Token> cur_token;

  Lexer(std::string path) : path(path), stream(path), line_number(1), line_pos(0) {
    if (!stream.is_open()) {
      std::cerr << "Could not open file " + path << std::endl;
      std::exit(1);
    }
    // load the first line
    std::getline(this->stream, this->line);
  }
  void advance();
};
