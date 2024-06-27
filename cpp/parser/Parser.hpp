#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <iostream>

#include "lexer/Lexer.hpp"

class AstNode {
protected:
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
  static bool ignore(Lexer &lexer, Token::Kind kind) {
    if (lexer.cur_token.kind == kind) {
      lexer.advance();
      return true;
    }
    return false;
  }

  static void log_node_start(const std::string &node_name) { std::cout << '<' + node_name + ">\n"; }
  static void log_node_end(const std::string &node_name) { std::cout << '<' + node_name + "/>\n"; }

public:
  virtual ~AstNode() = default;

  static AstNode *parse(Lexer &lexer);
};

#endif
