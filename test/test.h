#pragma once

#define TEST_MODULE_START(module)                                              \
  printf("\033[1mTesting module '%s'\033[0;37m\n", module)
#define TEST_MODULE_END(module)                                                \
  printf("\033[0;32mTest successful for module '%s' \033[0;37m\n", module)

void test_lexer(void);
void test_parser(void);
