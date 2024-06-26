#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class Expression : AstNode {
public:
  static Expression *parse(Lexer &lexer);
};

#endif
