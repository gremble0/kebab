#include <stdio.h>

#include "lexer.h"
#include "parser.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  lexer_t *lexer = lexer_init(argv[1]);
  ast_t *ast = parse(lexer);

  lexer_free(lexer);
  ast_free(ast);

  return 0;
}
