#include <memory>

#include "lexer/Lexer.hpp"
#include "parser/AndTest.hpp"
#include "parser/NotTest.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<AndTest> AndTest::parse(Lexer &lexer) {
  start_parsing("and-test");
  std::unique_ptr<AndTest> and_test = std::make_unique<AndTest>();

  // Keep parsing and tests until we have no longer ignored an `and` token
  while (true) {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    bool ignored_and = ignore(lexer, Token::Type::AND);
    if (!ignored_and)
      break;
  }

  end_parsing("and-test");
  return and_test;
}

void AndTest::compile(Compiler::Compiler &compiler) const {}

} // namespace Parser
} // namespace Kebab
