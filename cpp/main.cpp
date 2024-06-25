#include <iostream>
#include <ostream>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/RootNode.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  Lexer lexer(path);

  RootNode *root = RootNode::parse(lexer);

  Compiler compiler;
  compiler.compile(root);

  return 0;
}
