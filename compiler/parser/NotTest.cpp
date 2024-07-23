#include "parser/NotTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<NotTest> NotTest::parse(Lexer &lexer) {
  std::unique_ptr<NotTest> not_test = std::make_unique<NotTest>();
  not_test->start_parsing(lexer, "<not-test>");

  bool skipped_not = lexer.try_skip({Token::Type::NOT});
  if (skipped_not)
    not_test->is_negated = true;
  else
    not_test->is_negated = false;

  not_test->comparison = Comparison::parse(lexer);

  not_test->finish_parsing(lexer, "</not-test>");
  return not_test;
}

llvm::Value *NotTest::compile(Compiler &compiler) const {
  llvm::Value *comparison = this->comparison->compile(compiler);
  std::optional<llvm::Value *> operation;

  if (this->is_negated)
    operation = compiler.create_not(comparison);
  else
    operation = comparison;

  if (operation.has_value())
    return operation.value();
  else
    this->operator_error({compiler.get_bool_type()}, comparison->getType(), "~");
}

} // namespace Parser
} // namespace Kebab
