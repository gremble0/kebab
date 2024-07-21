#ifndef KEBAB_ROOTNODE_HPP
#define KEBAB_ROOTNODE_HPP

#include <memory>
#include <vector>

#include "compiler/Compiler.hpp"
#include "parser/AstNode.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

class RootNode : public AstNode {
public:
  std::vector<std::unique_ptr<Statement>> statements;

  static std::unique_ptr<RootNode> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

} // namespace Parser
} // namespace Kebab

#endif
