#ifndef KEBAB_LEXER_HPP
#define KEBAB_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <string>

#include "Token.hpp"

namespace Kebab {

class Lexer {
private:
  std::string path;
  std::string line;
  std::ifstream stream;
  size_t line_number;
  size_t line_pos;

  void next_line();
  Position position() const;
  uint8_t peek(int offset) const;
  [[noreturn]] void error(std::string message) const;

  Token read_number();
  Token read_char();
  Token read_string();
  Token read_word();

  void handle_one_char_type(Token::Type type);
  void handle_newline();
  void handle_whitespace();
  void handle_colon();
  void handle_equals();
  void handle_comma();
  void handle_lparen();
  void handle_rparen();
  void handle_lbracket();
  void handle_rbracket();
  void handle_plus();
  void handle_minus();
  void handle_mult();
  void handle_not();
  void handle_lt();
  void handle_gt();
  void handle_div();

public:
  Token cur_token;

  explicit Lexer(std::string path);
  void advance();
};

} // namespace Kebab

#endif
