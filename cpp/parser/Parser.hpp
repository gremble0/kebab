#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

static int indent_depth = 0;

class AstNode {
private:
  /**
   * @brief skip a token with an inner value and return it
   */
  template <typename T, Token::Kind K> static T skip_value(Lexer &lexer) {
    expect(lexer, K);
    T value = std::get<T>(lexer.cur_token.value);
    lexer.advance();
    return value;
  }

protected:
  AstNode() = default;

  [[noreturn]] static void error(const std::string &message) {
    std::cerr << "parser-error: " + message << std::endl;

    exit(1);
  }

  static void expect(Lexer &lexer, Token::Kind kind) {
    if (lexer.cur_token.kind != kind)
      error("unexpected token: '" + lexer.cur_token.to_string() + "' expected: '" +
            Token::kind_to_string(kind) + '\'');
  }

  // varargs?
  static void expect(Lexer &lexer, Token::Kind either, Token::Kind or_) {
    if (lexer.cur_token.kind != either && lexer.cur_token.kind != or_)
      error("unexpected token: '" + lexer.cur_token.to_string() + "' expected: '" +
            Token::kind_to_string(either) + "' or '" + Token::kind_to_string(or_) + '\'');
  }

  // TODO: make some more overloads - some times errors can be misleading if there are multiple
  // acceptable token kinds, e.g. `def a = list((list(int)) => [[1,2],[3,4])` (missing closing
  // bracket, but parser says expects comma)
  static void skip(Lexer &lexer, Token::Kind kind) {
    expect(lexer, kind);
    lexer.advance();
  }

  static void skip(Lexer &lexer, Token::Kind either, Token::Kind or_) {
    expect(lexer, either, or_);
    lexer.advance();
  }

  static int64_t skip_int(Lexer &lexer) {
    return skip_value<int64_t, Token::Kind::INT_LITERAL>(lexer);
  }

  static float_t skip_float(Lexer &lexer) {
    return skip_value<float_t, Token::Kind::FLOAT_LITERAL>(lexer);
  }

  static uint8_t skip_char(Lexer &lexer) {
    return skip_value<uint8_t, Token::Kind::CHAR_LITERAL>(lexer);
  }

  static std::string skip_string(Lexer &lexer) {
    return skip_value<std::string, Token::Kind::STRING_LITERAL>(lexer);
  }

  static std::string skip_name(Lexer &lexer) {
    return skip_value<std::string, Token::Kind::NAME>(lexer);
  }

  /**
   * @return whether the specified token was skipped
   */
  static bool ignore(Lexer &lexer, Token::Kind kind) {
    if (lexer.cur_token.kind == kind) {
      lexer.advance();
      return true;
    }
    return false;
  }

  static void start_parsing(const std::string &node_name) {
    std::string indent = std::string(indent_depth, ' ');
    std::cout << indent + '<' + node_name + ">\n";
    ++indent_depth;
  }

  static void end_parsing(const std::string &node_name) {
    --indent_depth;
    std::string indent = std::string(indent_depth, ' ');
    std::cout << indent + '<' + node_name + "/>\n";
  }

public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
};

std::unique_ptr<AstNode> parse(Lexer &lexer);

} // namespace Parser
} // namespace Kebab

#endif
