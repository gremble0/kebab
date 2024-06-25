#include "lexer/Lexer.hpp"

class AstNode {
  // virtual AstNode parse(Lexer lexer) = 0;
};

namespace Parser {

AstNode parse(Lexer lexer);

}
