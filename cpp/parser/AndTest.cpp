#include "AndTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/NotTest.hpp"

AndTest *AndTest::parse(Lexer &lexer) {
  log_node_start("and-test");
  AndTest *and_test = new AndTest();

  // Keep parsing and tests until we have no longer ignored an `and` token
  bool ignored_and;
  do {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    ignored_and = ignore(lexer, Token::Kind::AND);
  } while (!ignored_and);

  log_node_end("and-test");
  return and_test;
}
