#include <stdio.h>

#include "lexer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  lexer_t *lexer = lexer_init(argv[1]);

  token_t *cur_token = lexer_next_token(lexer);
  while (cur_token->kind != TOKEN_EOF) {
    cur_token = lexer_next_token(lexer);
  }

  lexer_free(lexer);

  return 0;
}
