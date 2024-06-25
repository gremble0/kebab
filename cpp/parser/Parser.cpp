#include "Parser.hpp"
#include "parser/Statement.hpp"
#include <memory>

std::unique_ptr<AstNode> Parser::parse(Lexer &lexer) {
  // asd
  return Statement::parse(lexer);
}
