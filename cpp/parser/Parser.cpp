#include "Parser.hpp"
#include "RootNode.hpp"
#include "Statement.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<AstNode> parse(Lexer &lexer) {
  std::unique_ptr<RootNode> root_node = std::make_unique<RootNode>();

  while (lexer.cur_token.kind != Token::Kind::END_OF_FILE)
    root_node->statements.push_back(Statement::parse(lexer));

  return root_node;
}

} // namespace Parser
} // namespace Kebab
