#ifndef KEBAB_ROOTNODE_HPP
#define KEBAB_ROOTNODE_HPP

#include <vector>

#include "parser/Parser.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

class RootNode : public AstNode {
public:
  std::vector<Statement *> statements;

  static RootNode *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
