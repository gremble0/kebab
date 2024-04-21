#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "utils.h"

static const char *actual_path = "keb-lexer.log";
static const char *expected_dir = "lexer-expected";
static const char *keb_dir = "lexer-keb";

/**
 * @brief Run the lexer on `keb_dir/base_file.keb` and assert that its file
 * contents are equal to `expected_dir/base_file.log`
 *
 * @param base_file base of file to test, will be expanded to derive the actual
 * file paths on the filesystem
 */
static void test_lexer_on_file(const char *base_file) {
  // Derive file locations of expected and source code files from `base_file`
  // NOTE: don't have to account for `/` because double strlen counts nullbyte
  // twice anyways, which means we will have enough size in each buffer
  char keb_path[strlen(keb_dir) + strlen(base_file) + sizeof(".keb")];
  char expected_path[strlen(expected_dir) + strlen(base_file) + sizeof(".log")];
  sprintf(keb_path, "%s/%s.keb", keb_dir, base_file);
  sprintf(expected_path, "%s/%s.log", expected_dir, base_file);

// LEXER_DEBUG must be defined for the lexer to log its output (not actually
// sure if this works, or if the lexer just cares about what was defined when it
// was compiled)
#ifndef LEXER_DEBUG
#define LEXER_DEBUG
#endif

  lexer_t *lexer = lexer_init(keb_path);

  FILE *actual_f = fopen(actual_path, "w+");
  if (actual_f == NULL)
    err_io_fail(actual_path);

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
