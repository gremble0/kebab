#include <memory>
#include <optional>

#include "lexer/Lexer.hpp"
#include "parser/AndTest.hpp"
#include "parser/NotTest.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<AndTest> AndTest::parse(Lexer &lexer) {
  std::unique_ptr<AndTest> and_test = std::make_unique<AndTest>();
  and_test->start_parsing(lexer, "<and-test>");

  // Keep parsing and tests until we have no longer ignored an `and` token
  while (true) {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    bool skipped_and = lexer.try_skip({Token::Type::AND});
    if (!skipped_and)
      break;
  }

  and_test->finish_parsing(lexer, "</and-test>");
  return and_test;
}

llvm::Value *AndTest::compile(Compiler &compiler) const {
  llvm::Value *result = this->not_tests.front()->compile(compiler);

  for (size_t i = 1; i < this->not_tests.size(); ++i) {
    llvm::Value *rhs = this->not_tests[i]->compile(compiler);
    std::optional<llvm::Value *> operation = compiler.create_and(result, rhs);
    if (operation.has_value())
      result = operation.value();
    else
      this->operator_error({compiler.get_bool_type()}, result->getType(), "and");
  }

  return result;
}

} // namespace Parser
} // namespace Kebab
