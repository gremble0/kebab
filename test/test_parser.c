#include <stdio.h>
#include <string.h>

#include "lexer/lexer.h"
#include "nonstdlib/nerror.h"
#include "parser/parser.h"
#include "utils.h"

static void test_parser_on_file(const char *base_file) {
  char keb_file_path[strlen(keb_dir_path) + strlen(base_file) + sizeof(".keb") +
                     1];
  char expected_path[strlen(parser_expected_dir_path) + strlen(base_file) +
                     sizeof(".log") + 1];
  sprintf(keb_file_path, "%s/%s.keb", keb_dir_path, base_file);
  sprintf(expected_path, "%s/%s.ast", parser_expected_dir_path, base_file);

  lexer_t *lexer = lexer_init(keb_file_path);
  ast_t *ast = parse(lexer);

  FILE *actual_f = fopen(parser_log_file_path, "r");
  if (actual_f == NULL)
    err_io_fail(parser_log_file_path);

  FILE *expected_f = fopen(expected_path, "r");
  if (expected_f == NULL)
    err_io_fail(expected_path);

  assert_file_contents_equal(actual_f, expected_f);

  lexer_free(lexer);
  ast_free(ast);
}

void test_parser(void) {
  TEST_MODULE_START("parser");

  test_parser_on_file("constructors");
  test_parser_on_file("comparisons");
  test_parser_on_file("operators");
  test_parser_on_file("comments");

  TEST_MODULE_END("parser");
}
