#include "parser/NotTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Expression.hpp"
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

} // namespace Kebab::Parser
