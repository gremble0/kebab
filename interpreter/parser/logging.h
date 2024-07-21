#pragma once

#include <stdio.h>

#include "parser/error.h"

#ifdef DEBUG_PARSER
#define PARSER_LOG_START() parser_log_start();
#define PARSER_LOG_FINISH() parser_log_finish();
#define PARSER_LOG_NODE_START(node, ...) parser_log_node_start(node, ##__VA_ARGS__)
#define PARSER_LOG_NODE_FINISH(node, ...) parser_log_node_finish(node, ##__VA_ARGS__)
#define PARSER_LOG_NODE_SELF_CLOSING(node, ...) parser_log_node_self_closing(node, ##__VA_ARGS__)
#else
#define PARSER_LOG_START(...)
#define PARSER_LOG_FINISH(...)
#define PARSER_LOG_NODE_START(node, ...)
#define PARSER_LOG_NODE_FINISH(node, ...)
#define PARSER_LOG_NODE_SELF_CLOSING(node, ...)
#endif

#define EXPECT_TOKEN(lexer, expected)                                                              \
  do {                                                                                             \
    if (lexer->cur_token->kind != expected) {                                                      \
      err_wrong_token(lexer, token_kind_to_string(expected), token_to_string(lexer->cur_token));   \
    }                                                                                              \
  } while (0)

#define SKIP_TOKEN(lexer, expected)                                                                \
  do {                                                                                             \
    EXPECT_TOKEN(lexer, expected);                                                                 \
    lexer_advance(lexer);                                                                          \
  } while (0)

static size_t indent_depth = 0;
static FILE *log_file;

void parser_log_start();
void parser_log_finish();
void parser_log_node_start(const char *node_name, ...) __attribute__((format(printf, 1, 2)));
void parser_log_node_finish(const char *node_name, ...) __attribute__((format(printf, 1, 2)));
void parser_log_node_self_closing(const char *node_name, ...) __attribute__((format(printf, 1, 2)));
