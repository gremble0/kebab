#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "utils.h"

/**
 * @brief Run the lexer on `keb_path/base_file.keb` and assert that its file
 * contents are equal to `lexer_expected_path/base_file.log`
 *
 * @param base_file base of file to test, will be expanded to derive the actual
 * file paths on the filesystem
 */
static void test_lexer_on_file(const char *base_file) {
  // Derive file locations of expected and source code files from `base_file`
  // NOTE: don't have to account for `/` because double strlen counts nullbyte
  // twice anyways, which means we will have enough size in each buffer
  char keb_file_path[strlen(keb_dir_path) + strlen(base_file) + sizeof(".keb") +
                     1];
  char expected_path[strlen(lexer_expected_dir_path) + strlen(base_file) +
                     sizeof(".log") + 1];
  sprintf(keb_file_path, "%s/%s.keb", keb_dir_path, base_file);
  sprintf(expected_path, "%s/%s.lex", lexer_expected_dir_path, base_file);

  lexer_t *lexer = lexer_init(keb_file_path);

  FILE *actual_f = fopen(lexer_log_file_path, "r");
  if (actual_f == NULL)
    err_io_fail(lexer_log_file_path);

  FILE *expected_f = fopen(expected_path, "r");
  if (expected_f == NULL)
    err_io_fail(expected_path);

  // Lex the source file
  while (lexer->cur_token->kind != TOKEN_EOF)
    lexer_advance(lexer);

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
