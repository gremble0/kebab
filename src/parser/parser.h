#pragma once

#include "lexer.h"
#include "nlist.h"

// Entrypoint to build/use the contents of a program
typedef struct {
  list_t *stmts; // list<statement_t>
  list_t *strs;  // list<char*> string representation of the ast (only used in
                 // debug mode)
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
void build_strs(ast_t *ast);
