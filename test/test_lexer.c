#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "nerror.h"
#include "token.h"

// Move to lexer?
static int lexer_write_cur_token(lexer_t *lexer, FILE *f) {
  token_t *token = lexer_next_token(lexer);
  token_kind_t token_kind = token->kind;

  char *token_string = token_to_string(token);
  int token_string_len = strlen(token_string);
  fwrite(token_string, 1, token_string_len, f);
  fwrite("\n", 1, 1, f);

  free(token_string);
  token_free(token);

  if (token_kind == TOKEN_EOF) {
    return 1;
  } else {
    return 0;
  }
}

static void assert_file_contents_equal(FILE *f1, FILE *f2) {}

void test_lexer() {
  lexer_t *lexer = lexer_init("./keb/test-lexer.keb");
  FILE *actual = fopen("./keb/test-lexer.txt", "w");

  while (lexer_write_cur_token(lexer, actual) != 1)
    ;

  FILE *expected = fopen("./keb/test-lexer-expected.txt", "r");

  assert_file_contents_equal(actual, expected);

  fclose(actual);
  fclose(expected);

  lexer_free(lexer);
}
