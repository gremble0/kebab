#include "Parser.hpp"
#include "RootNode.hpp"
#include "Statement.hpp"

namespace Parser {

AstNode *parse(Lexer &lexer) {
  RootNode *root_node = new RootNode();

  while (lexer.cur_token.kind != Token::Kind::END_OF_FILE)
    root_node->statements.push_back(Statement::parse(lexer));

  return root_node;
}
} // namespace Parser
