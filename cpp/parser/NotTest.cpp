#include "parser/NotTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<NotTest> NotTest::parse(Lexer &lexer) {
  std::unique_ptr<NotTest> not_test = std::make_unique<NotTest>();
  not_test->start_parsing(lexer, "<not-test>");

  if (lexer.cur_token->type == Token::Type::NOT) {
    not_test->is_negated = true;
    lexer.advance();
  } else {
    not_test->is_negated = false;
  }

  not_test->comparison = Comparison::parse(lexer);

  not_test->finish_parsing(lexer, "</not-test>");
  return not_test;
}

llvm::Value *NotTest::compile(Compiler &compiler) const {
  // TODO: some not logic (this->is_negated)
  return this->comparison->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
