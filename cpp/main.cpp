#include <iostream>
#include <memory>
#include <ostream>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <file.keb> <args>" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  Lexer lexer(path);

  while (lexer.cur_token.kind != Token::Kind::END_OF_FILE) {
    lexer.advance();
    std::cout << lexer.cur_token.to_string() << std::endl;
  }

  std::unique_ptr<AstNode> root = Parser::parse(lexer);

  Compiler compiler;
  compiler.compile(root.get());

  return 0;
}
