#pragma once

// TODO: abbreviate references to generic types e.g. definition -> def?
// TODO: list_literal [1, 2, 3] ?

#include "lexer.h"
#include "nlist.h"

// Entrypoint to build/use the contents of a program
typedef struct ast_t {
  list_t *statements; // list<statement_t>
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
