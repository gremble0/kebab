#include <stdio.h>

#include "lexer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  lexer_t *lexer = lexer_init(argv[1]);
  lexer_free(lexer);

  return 0;
}
