#include <variant>

#include "compiler/Errors.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Expression.hpp"
#include "parser/NotTest.hpp"
#include "parser/Statement.hpp"

namespace Kebab::Parser {

std::unique_ptr<NotTest> NotTest::parse(Lexer &lexer) {
  auto not_test = std::make_unique<NotTest>();
  not_test->start_parsing(lexer, "<not-test>");

  if (bool skipped_not = lexer.try_skip({Token::Type::NOT}))
    not_test->is_negated = true;
  else
    not_test->is_negated = false;

  not_test->comparison = Comparison::parse(lexer);

  not_test->finish_parsing(lexer, "</not-test>");
  return not_test;
}

llvm::Value *NotTest::compile(Compiler &compiler) const {
  llvm::Value *comparison_compiled = this->comparison->compile(compiler);

  auto create_operation =
      [this, &compiler, &comparison_compiled]() -> std::variant<llvm::Value *, UnaryOperatorError> {
    if (this->is_negated)
      return compiler.create_not(comparison_compiled);
    else
      return comparison_compiled;
  };

  std::variant<llvm::Value *, UnaryOperatorError> operation = create_operation();

  if (std::holds_alternative<llvm::Value *>(operation))
    return std::get<llvm::Value *>(operation);
  else
    this->compiler_error(std::get<UnaryOperatorError>(operation));
}

} // namespace Kebab::Parser
