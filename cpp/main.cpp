#include <iostream>
#include <ostream>

#include "lexer/Lexer.hpp"

using namespace Kebab;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  Lexer lexer(path);
  lexer.advance();

  while (lexer.cur_token.kind != Token::END_OF_FILE) {
    std::cout << lexer.cur_token.to_string() << std::endl;
    lexer.advance();
  }

  return 0;
}
