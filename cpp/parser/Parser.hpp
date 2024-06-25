#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>

#include "lexer/Lexer.hpp"

class AstNode {
public:
  virtual ~AstNode() = default;

  static AstNode *parse(Lexer &lexer);

  [[noreturn]] static void error(const std::string &message) {
    std::cerr << "parser-error: " + message + '\n';

    exit(1);
  }
};

namespace Parser {

AstNode *parse(Lexer &lexer);

}

#endif
