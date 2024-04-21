#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "test.h"

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

/**
 * @param base_file the base of the file to be tested, will be formatted into
 * three separate files:
 *
 * 1: base-file.keb
 * 2: base-file-expected.txt
 * 3: base-file-actual.txt.
 *
 * Out of these files, base-file.keb and base-file-expected.txt should be
 * present under `base_dir` before running this function. The function will
 * generate the base-file-actual.txt file
 */
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
  if (actual_f == NULL)
    err_io_fail(actual_path);

  FILE *expected_f = fopen(expected_path, "r");
  if (expected_f == NULL)
    err_io_fail(expected_path);

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
  TEST_MODULE_START("lexer");

  test_lexer_on_file("constructors");
  test_lexer_on_file("comparisons");
  test_lexer_on_file("operators");
  test_lexer_on_file("comments");

  TEST_MODULE_END("lexer");
}
