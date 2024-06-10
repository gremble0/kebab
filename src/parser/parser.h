#pragma once

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"

// Entrypoint to build/use the contents of a program
typedef struct {
  list_t *stmts; // list<statement_t *>
} ast_t;

typedef struct {
  size_t line;
  size_t col;
} position_t;

typedef struct {
  position_t start;
  position_t end;
  file_t file;
} span_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
