#include "parser/RootNode.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<RootNode> RootNode::parse(Lexer &lexer) {
  std::unique_ptr<RootNode> root_node = std::make_unique<RootNode>();

  while (lexer.cur_token.type != Token::Type::END_OF_FILE)
    root_node->statements.push_back(Statement::parse(lexer));

  return root_node;
}

void RootNode::compile(Compiler::Compiler &compiler) {
  for (std::unique_ptr<Statement> const &statement : this->statements)
    statement->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
