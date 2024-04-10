#pragma once

// TODO: abbreviate references to generic types e.g. definition -> def?

#include "lexer.h"
#include "nlist.h"

// Entrypoint to build/use the contents of a program
typedef struct {
  list_t *stmts; // list<statement_t>
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
