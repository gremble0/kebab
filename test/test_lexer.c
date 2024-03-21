#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "lexer.h"
#include "token.h"

void test_lexer() {
  lexer_t *lexer = lexer_init("./keb/test-lexer.keb");
  token_t *t = lexer_next_token(lexer);

  char *s = token_to_string(t);
  printf("%s\n", s);
  free(s);
}
