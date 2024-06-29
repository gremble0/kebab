#ifndef KEBAB_ROOTNODE_HPP
#define KEBAB_ROOTNODE_HPP

#include <memory>
#include <vector>

#include "parser/Parser.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

class RootNode : public AstNode {
public:
  std::vector<std::unique_ptr<Statement>> statements;

  static std::unique_ptr<RootNode> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
