#include <iostream>
#include <ostream>

#include "lexer/Lexer.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  Lexer lexer(path);

  while (lexer.cur_token.kind != TokenKind::TOKEN_EOF) {
    lexer.advance();
    std::cout << lexer.cur_token.to_string() << std::endl;
  }

  return 0;
}
