#ifndef KEBAB_KEBAB_ANDTEST_HPP
#define KEBAB_KEBAB_ANDTEST_HPP

#include <memory>
#include <vector>

#include "NotTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class AndTest : public AstNode {
public:
  std::vector<std::unique_ptr<NotTest>> not_tests;

  static std::unique_ptr<AndTest> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

} // namespace Parser
} // namespace Kebab

#endif
