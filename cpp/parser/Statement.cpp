#include "Statement.hpp"
#include <memory>

std::unique_ptr<Statement> Statement::parse(Lexer &lexer) {
  // aaa
  return std::make_unique<Statement>();
}
