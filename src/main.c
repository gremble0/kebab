#include <stdio.h>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "runtime/runtime.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  lexer_t *lexer = lexer_init(argv[1]);
  ast_t *ast = parse(lexer);
  eval(ast);

  lexer_free(lexer);
  free_ast(ast);

  return 0;
}
