#pragma once

#include "error.h"
#include "nonstdlib/nerror.h"

#ifdef DEBUG_PARSER
#define START_PARSING(section) start_parsing(section)
#define FINISH_PARSING(section) finish_parsing(section)
#define START_AND_FINISH_PARSING(section) start_and_finish_parsing(section)
#else
#define START_PARSING(section)
#define FINISH_PARSING(section)
#define START_AND_FINISH_PARSING(section)
#endif

#define ONE_INDENT "  "
#define LIST_START_SIZE 5
#define EXPECT_TOKEN(expected, lexer)                                          \
  if (lexer->cur_token->kind != expected) {                                    \
    err_wrong_token(token_kind_to_string(expected), lexer);                    \
  }

#define SKIP_TOKEN(expected, lexer)                                            \
  do {                                                                         \
    EXPECT_TOKEN(expected, lexer);                                             \
    lexer_advance(lexer);                                                      \
  } while (0)

static size_t indent_depth = 0;

void start_parsing(const char *node_name);
void finish_parsing(const char *node_name);
void start_and_finish_parsing(const char *node_name);
char *repeat_char(char c, size_t n);
