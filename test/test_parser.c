#include "lexer/lexer.h"
#include "parser/parser.h"
#include "utils.h"

void test_parser_on_file(const char *base_file) {}

void test_parser(void) {
  TEST_MODULE_START("parser");

  ast_t *ast = parse(lexer_init("./parser/constructors.keb"));

  TEST_MODULE_END("parser");
}
