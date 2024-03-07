#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  lexer_t *lexer = lexer_init(argv[1]);

  while (1) {
    token_t *token = lexer_next_token(lexer);

    if (token->kind == TOKEN_EOF) {
      free(token);
      break;
    } else {
      token_print(token);
      token_free(token);
    }
  }

  lexer_free(lexer);

  return 0;
}
