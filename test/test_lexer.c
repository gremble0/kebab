#include "nerror.h"
#include "lexer.h"
#include "token.h"

void test_lexer() {
  lexer_t *lexer = lexer_init("./keb/test-lexer.keb");

  token_t *t = lexer_next_token(lexer);
  ASSERT(t->kind == TOKEN_DEF);
}
