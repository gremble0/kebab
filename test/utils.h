#pragma once

#include <stdio.h>

#define TEST_MODULE_START(module)                                              \
  printf("\033[1mTesting module '%s'\033[0;37m\n", module)
#define TEST_MODULE_END(module)                                                \
  printf("\033[0;32mTest successful for module '%s' \033[0;37m\n", module)

static const char *lexer_log_file_path = "keb-lexer.log";
static const char *lexer_expected_dir_path = "lexer-expected";
static const char *parser_log_file_path = "keb-parser.log";
static const char *parser_expected_dir_path = "parser-expected";
static const char *keb_dir_path = "keb";

void test_lexer(void);
void test_parser(void);
void assert_file_contents_equal(FILE *f1, FILE *f2);
