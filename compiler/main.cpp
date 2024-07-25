#include <iostream>
#include <memory>
#include <ostream>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/RootNode.hpp"

using namespace Kebab;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);

  Lexer lexer(path);
  std::unique_ptr<Parser::RootNode> root = Parser::RootNode::parse(lexer);

  Compiler compiler;
  compiler.compile(std::move(root));

  return 0;
}
