#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Parser.hpp"
#include "lexer/Lexer.hpp"

class Statement : AstNode {
  static Statement parse(Lexer lexer);
};

#endif
