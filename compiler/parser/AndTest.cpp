#include <memory>
#include <variant>

#include "compiler/Errors.hpp"
#include "lexer/Lexer.hpp"
#include "parser/AndTest.hpp"
#include "parser/Expression.hpp"
#include "parser/NotTest.hpp"
#include "parser/Statement.hpp"

namespace Kebab::Parser {

std::unique_ptr<AndTest> AndTest::parse(Lexer &lexer) {
  auto and_test = std::make_unique<AndTest>();
  and_test->start_parsing(lexer, "<and-test>");

  // Keep parsing and tests until we have no longer ignored an `and` token
  while (true) {
    and_test->not_tests.push_back(NotTest::parse(lexer));

    if (bool skipped_and = lexer.try_skip({Token::Type::AND}); !skipped_and)
      break;
  }

  and_test->finish_parsing(lexer, "</and-test>");
  return and_test;
}

llvm::Value *AndTest::compile(Compiler &compiler) const {
  llvm::Value *result = this->not_tests.front()->compile(compiler);

  for (size_t i = 1; i < this->not_tests.size(); ++i) {
    llvm::Value *rhs = this->not_tests[i]->compile(compiler);
    std::variant<llvm::Value *, BinaryOperatorError> operation = compiler.create_and(result, rhs);
    if (std::holds_alternative<llvm::Value *>(operation))
      result = std::get<llvm::Value *>(operation);
    else
      this->compiler_error(std::get<BinaryOperatorError>(operation));
  }

  return result;
}

} // namespace Kebab::Parser
