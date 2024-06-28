#ifndef KEBAB_KEBAB_ANDTEST_HPP
#define KEBAB_KEBAB_ANDTEST_HPP

#include <vector>

#include "NotTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class AndTest : AstNode {
public:
  std::vector<NotTest *> not_tests;

  static AndTest *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
