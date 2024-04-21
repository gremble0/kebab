#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "test.h"

static const char *base_dir = "./parser/";

void test_parser_on_file(const char *base_file) {}

void test_parser(void) {
  TEST_MODULE_START("parser");

  // test_parser_on_file("constructors");
  ast_t *ast = parse(lexer_init("./parser/constructors.keb"));
  build_strs(ast);

  size_t indent_width = 0;
  for (size_t i = 0; i < ast->strs->cur_size; ++i) {
    printf("%s\n", (char *)list_get(ast->strs, i));
  }

  TEST_MODULE_END("parser");
}
