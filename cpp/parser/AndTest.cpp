#include "AndTest.hpp"
#include "NotTest.hpp"
#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

AndTest *AndTest::parse(Lexer &lexer) {
  start_parsing("and-test");
  AndTest *and_test = new AndTest();

  // Keep parsing and tests until we have no longer ignored an `and` token
  while (true) {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    bool ignored_and = ignore(lexer, Token::Kind::AND);
    if (!ignored_and)
      break;
  }

  end_parsing("and-test");
  return and_test;
}

} // namespace Parser
} // namespace Kebab
