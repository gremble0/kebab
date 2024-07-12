#include "parser/RootNode.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<RootNode> RootNode::parse(Lexer &lexer) {
  std::unique_ptr<RootNode> root_node = std::make_unique<RootNode>();
  root_node->start_parsing(lexer, "<root>");

  // TODO: make lexer.eof() or lexer.finished() something like this
  while (lexer.peek()->type != Token::Type::END_OF_FILE)
    root_node->statements.push_back(Statement::parse(lexer));

  root_node->finish_parsing(lexer, "</root>");
  return root_node;
}

llvm::Value *RootNode::compile(Compiler &compiler) const {
  for (std::unique_ptr<Statement> const &statement : this->statements)
    statement->compile(compiler);

  // meh
  return nullptr;
}

} // namespace Parser
} // namespace Kebab
