#ifndef KEBAB_ROOTNODE_HPP
#define KEBAB_ROOTNODE_HPP

#include <vector>

#include "parser/Parser.hpp"
#include "parser/Statement.hpp"

class RootNode : public AstNode {
public:
  std::vector<Statement *> statements;

  static RootNode *parse(Lexer &lexer);
};

#endif
