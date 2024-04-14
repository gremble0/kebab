#include "lexer.h"
#include "parser.h"
#include "test.h"

static const char *base_dir = "./parser/";

void test_parser_on_file(const char *base_file) {}

void test_parser(void) {
  TEST_MODULE_START("parser");

  // test_parser_on_file("constructors");
  ast_t *ast = parse(lexer_init("./parser/constructors.keb"));

  TEST_MODULE_END("parser");
}
