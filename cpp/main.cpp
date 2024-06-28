#include <iostream>
#include <ostream>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

using namespace Kebab;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  Lexer lexer(path);

  Parser::parse(lexer);

  return 0;
}
