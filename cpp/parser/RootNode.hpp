#ifndef KEBAB_ROOTNODE_HPP
#define KEBAB_ROOTNODE_HPP

#include <memory>
#include <vector>

#include "compiler/Compiler.hpp"
#include "parser/Parser.hpp"
#include "parser/Statement.hpp"

namespace Kebab {

namespace Parser {

class RootNode : public AstNode {
public:
  std::vector<std::unique_ptr<Statement>> statements;

  static std::unique_ptr<RootNode> parse(Lexer &lexer);
  void compile(Compiler::Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
