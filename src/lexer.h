#pragma once

#include <stdio.h>

#include "token.h"

typedef struct lexer_t {
  FILE *source_file;
  char *line;
  ssize_t line_len;
  size_t line_pos;
} lexer_t;

// TODO: make void lexer_load_token and make lexer hold a cur_token ?
lexer_t *lexer_init(const char *file_path);
token_t *lexer_next_token(lexer_t *lexer);
token_t *lexer_peek_token(lexer_t *lexer);
void lexer_skip_token(lexer_t *lexer, token_kind_t token_kind);
void lexer_free(lexer_t *lexer);
