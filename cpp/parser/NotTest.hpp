#ifndef KEBAB_KEBAB_NOTTEST_HPP
#define KEBAB_KEBAB_NOTTEST_HPP

#include "Comparison.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class NotTest : public AstNode {
public:
  bool is_negated;
  std::unique_ptr<Comparison> comparison;

  static std::unique_ptr<NotTest> parse(Lexer &lexer);
  void compile(Compiler::Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
