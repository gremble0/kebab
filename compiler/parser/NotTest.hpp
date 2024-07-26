#ifndef KEBAB_KEBAB_NOTTEST_HPP
#define KEBAB_KEBAB_NOTTEST_HPP

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Comparison.hpp"

namespace Kebab::Parser {

class NotTest : public AstNode {
public:
  bool is_negated;
  std::unique_ptr<Comparison> comparison;

  static std::unique_ptr<NotTest> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

} // namespace Kebab::Parser

#endif
