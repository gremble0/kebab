#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "nerror.h"
#include "token.h"

static const char *base_dir = "./lexer/";

static void lexer_write_cur_token(lexer_t *lexer, FILE *f) {
  char *token_string = token_to_string(lexer->cur_token);
  int token_string_len = strlen(token_string);
  fwrite(token_string, 1, token_string_len, f);
  fwrite("\n", 1, 1, f);

  free(token_string);
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

static void test_lexer_on_file(const char *base_file) {
  char keb_path[strlen(base_dir) + strlen(base_file) + sizeof(".keb")];
  sprintf(keb_path, "%s%s.keb", base_dir, base_file);

  char
      actual_path[strlen(base_dir) + strlen(base_file) + sizeof("-actual.txt")];
  sprintf(actual_path, "%s%s-actual.txt", base_dir, base_file);

  char expected_path[strlen(base_dir) + strlen(base_file) +
                     sizeof("-expected.txt")];
  sprintf(expected_path, "%s%s-expected.txt", base_dir, base_file);

  lexer_t *lexer = lexer_init(keb_path);

  FILE *actual_f = fopen(actual_path, "w+");
  if (actual_f == NULL) {
    printf("%s\n", actual_path);
    err_io_fail(actual_path);
  }

  FILE *expected_f = fopen(expected_path, "r");
  if (expected_f == NULL) {
    err_io_fail(expected_path);
  }

  while (lexer->cur_token->kind != TOKEN_EOF) {
    lexer_write_cur_token(lexer, actual_f);
    lexer_advance(lexer);
  }

  // Reset the new file to the start and assert its contents are the same as the
  // expected lexed version
  fseek(actual_f, 0, SEEK_SET);
  assert_file_contents_equal(actual_f, expected_f);

  fclose(actual_f);
  fclose(expected_f);
  lexer_free(lexer);
}

void test_lexer(void) {
  test_lexer_on_file("constructors");
  test_lexer_on_file("comparisons");
  test_lexer_on_file("operators");
  test_lexer_on_file("comments");
}
