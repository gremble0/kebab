#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer/Lexer.hpp"

class AstNode {
  // virtual AstNode parse(Lexer lexer) = 0;
};

namespace Parser {

AstNode parse(Lexer lexer);

}

#endif
