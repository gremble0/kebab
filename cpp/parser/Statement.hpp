#include "Parser.hpp"
#include "lexer/Lexer.hpp"

class Statement : AstNode {
  static Statement parse(Lexer lexer);
};
