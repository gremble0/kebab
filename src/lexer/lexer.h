#pragma once

#include <stdio.h>

#include "token.h"

typedef struct lexer_t {
  FILE *source_file;
  char *line;
  ssize_t line_len;
  size_t line_pos;
  token_t *cur_token;
} lexer_t;

// TODO: make void lexer_load_token and make lexer hold a cur_token ?
lexer_t *lexer_init(const char *file_path);
void lexer_advance(lexer_t *lexer);
void lexer_free(lexer_t *lexer);
