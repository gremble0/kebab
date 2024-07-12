#ifndef KEBAB_LEXER_HPP
#define KEBAB_LEXER_HPP

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>

#include "Token.hpp"

namespace Kebab {

class Lexer {
private:
  const std::string path;
  std::string line;
  std::ifstream stream;
  size_t line_number = 0;
  size_t line_pos = 0;
  std::shared_ptr<Token> token;

  void next_line();
  uint8_t peek(int offset) const;
  [[noreturn]] void error(const std::string &message) const;

  uint8_t read_maybe_escaped_char();

  std::unique_ptr<Token> read_number();
  std::unique_ptr<Token> read_char();
  std::unique_ptr<Token> read_string();
  std::unique_ptr<Token> read_word();

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

  template <typename T, Token::Type K> T skip_value(Lexer &lexer) {
    this->expect(K);
    T value = std::get<T>(lexer.peek()->value);
    lexer.advance();
    return value;
  }

public:
  explicit Lexer(const std::string &path);

  void advance();
  std::shared_ptr<Token> peek() const { return this->token; }

  void expect(Token::Type expected) const;
  void expect(Token::Type either, Token::Type or_) const;
  void skip(Token::Type expected);
  void skip(Token::Type either, Token::Type or_);
  bool ignore(Token::Type type);

  int64_t skip_int(Lexer &lexer) { return skip_value<int64_t, Token::Type::INT_LITERAL>(lexer); }

  float_t skip_float(Lexer &lexer) {
    return skip_value<float_t, Token::Type::FLOAT_LITERAL>(lexer);
  }

  uint8_t skip_char(Lexer &lexer) { return skip_value<uint8_t, Token::Type::CHAR_LITERAL>(lexer); }

  std::string skip_string(Lexer &lexer) {
    return skip_value<std::string, Token::Type::STRING_LITERAL>(lexer);
  }

  std::string skip_name(Lexer &lexer) { return skip_value<std::string, Token::Type::NAME>(lexer); }

  std::string pretty_position() const;
  Position position() const { return Position(this->line_number, this->line_pos); };
  const std::string &get_path() const { return this->path; };
};

} // namespace Kebab

#endif
