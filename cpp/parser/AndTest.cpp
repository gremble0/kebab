#include <memory>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/AndTest.hpp"
#include "parser/NotTest.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<AndTest> AndTest::parse(Lexer &lexer) {
  Logger::start_parsing("and-test");
  std::unique_ptr<AndTest> and_test = std::make_unique<AndTest>();

  // Keep parsing and tests until we have no longer ignored an `and` token
  while (true) {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    bool ignored_and = ignore(lexer, Token::Type::AND);
    if (!ignored_and)
      break;
  }

  Logger::end_parsing("and-test");
  return and_test;
}

llvm::Value *AndTest::compile(Compiler &compiler) const {
  // for (std::unique_ptr<NotTest> const &not_test : this->not_tests)
  return not_tests.at(0)->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
