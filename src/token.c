#include <stdlib.h>

#include "error.h"
#include "token.h"

token_t *lexer_make_eof_token() {
  token_t *eof = malloc(sizeof(token_t));
  if (eof == NULL) {
    err_malloc_fail();
  }
  eof->kind = TOKEN_EOF;

  return eof;
}
