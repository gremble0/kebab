#pragma once

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"

// Entrypoint to build/use the contents of a program
typedef struct {
  list_t *stmts; // list<statement_t *>
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
