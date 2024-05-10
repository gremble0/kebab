#pragma once

#include <stdio.h>

#include "token.h"

typedef struct {
  FILE *f;
  const char *name; // used for error handling
} file_t;

typedef struct {
  file_t *source_file;
  char *line;
  ssize_t line_len;
  size_t prev_line_pos; // start position of previous token (used for error
                        // handling)
  size_t line_pos;      // start position of current token
  size_t line_number;   // current line number in source file
  token_t *cur_token;
} lexer_t;

// TODO: make void lexer_load_token and make lexer hold a cur_token ?
lexer_t *lexer_init(const char *path);
void lexer_advance(lexer_t *lexer);
void lexer_free(lexer_t *lexer);
