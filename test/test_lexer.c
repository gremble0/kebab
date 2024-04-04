#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "nerror.h"
#include "token.h"

// Move to lexer?
static int lexer_write_cur_token(lexer_t *lexer, FILE *f) {
  // token_t *token = lexer_next_token(lexer);
  lexer_advance(lexer);

  char *token_string = token_to_string(lexer->cur_token);
  int token_string_len = strlen(token_string);
  fwrite(token_string, 1, token_string_len, f);
  fwrite("\n", 1, 1, f);

  int ret;
  if (lexer->cur_token->kind == TOKEN_EOF) {
    ret = 1;
  } else {
    ret = 0;
  }

  free(token_string);
  // token_free(lexer->cur_token);

  return ret;
}

static void assert_file_contents_equal(FILE *f1, FILE *f2) {
  char buffer1[BUFSIZ];
  char buffer2[BUFSIZ];

  while (!feof(f1) && !feof(f2)) {
    size_t read1 = fread(buffer1, 1, BUFSIZ, f1);
    size_t read2 = fread(buffer2, 1, BUFSIZ, f2);

    ASSERT(read1 == read2);

    ASSERT(memcmp(buffer1, buffer2, read1) == 0);
  }
}

static void test_lexer_on_file(const char *path, const char *actual_path,
                               const char *expected_path) {
  // Lex the source code file and log it to a txt file
  lexer_t *lexer = lexer_init(path);

  FILE *actual = fopen(actual_path, "w+");
  if (actual == NULL) {
    err_io_fail("");
  }

  FILE *expected = fopen(expected_path, "r");
  if (expected == NULL) {
    err_io_fail("");
  }

  while (lexer_write_cur_token(lexer, actual) != 1)
    ;

  // Reset the new file to the start and assert its contents are the same as the
  // expected lexed version
  fseek(actual, 0, SEEK_SET);
  assert_file_contents_equal(actual, expected);

  fclose(actual);
  fclose(expected);
  lexer_free(lexer);
}

void test_lexer() {
  test_lexer_on_file("./lexer/test-lexer.keb", "./lexer/test-lexer.txt",
                     "./lexer/test-lexer-expected.txt");
}
