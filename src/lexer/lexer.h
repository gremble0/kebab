#pragma once

#include <stdio.h>

#include "token.h"

typedef struct source_file_t {
  FILE *f;
  const char *f_name; // used for error handling
} source_file_t;

typedef struct lexer_t {
  source_file_t *source_file;
  char *line;
  ssize_t line_len;
  size_t prev_pos; // start position of previous token (used for error handling)
  size_t line_pos; // start position of current token
  size_t line_number; // current line number in source file
  token_t *cur_token;
} lexer_t;

// TODO: make void lexer_load_token and make lexer hold a cur_token ?
lexer_t *lexer_init(const char *file_path);
void lexer_advance(lexer_t *lexer);
void lexer_free(lexer_t *lexer);
