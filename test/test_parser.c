#include <string.h>

#include "lexer/lexer.h"
#include "nonstdlib/nerror.h"
#include "parser/parser.h"
#include "utils.h"

static const char *actual_path = "keb-parser.log";
static const char *expected_dir = "parser-expected";
static const char *keb_dir = "keb";

void test_parser_on_file(const char *base_file) {
  char keb_path[strlen(keb_dir) + strlen(base_file) + sizeof(".keb")];
  char expected_path[strlen(expected_dir) + strlen(base_file) + sizeof(".log")];
  sprintf(keb_path, "%s/%s.keb", keb_dir, base_file);
  sprintf(expected_path, "%s/%s.log", expected_dir, base_file);

  lexer_t *lexer = lexer_init(keb_path);
  ast_t *ast = parse(lexer);

  FILE *actual_f = fopen(actual_path, "r");
  if (actual_f == NULL)
    err_io_fail(actual_path);

  FILE *expected_f = fopen(expected_path, "r");
  if (expected_f == NULL)
    err_io_fail(expected_path);

  assert_file_contents_equal(actual_f, expected_f);

  lexer_free(lexer);
  ast_free(ast);
}

void test_parser(void) {
  TEST_MODULE_START("parser");

  test_parser_on_file("comments");

  TEST_MODULE_END("parser");
}
