#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <iostream>

#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

class AstNode {
protected:
  AstNode() = default;

  [[noreturn]] static void error(const std::string &message) {
    std::cerr << "parser-error: " + message + '\n';

    exit(1);
  }

  static void expect(Lexer &lexer, Token::Kind kind) {
    if (lexer.cur_token.kind != kind)
      error("unexpected token '" + lexer.cur_token.to_string() + '\'');
  }

  static void skip(Lexer &lexer, Token::Kind kind) {
    expect(lexer, kind);
    lexer.advance();
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

  static void start_parsing(const std::string &node_name) { std::cout << '<' + node_name + ">\n"; }
  static void end_parsing(const std::string &node_name) { std::cout << '<' + node_name + "/>\n"; }

public:
  virtual ~AstNode() = default;

  static AstNode *parse(Lexer &lexer);
};

AstNode *parse(Lexer &lexer);

} // namespace Parser
} // namespace Kebab

#endif
