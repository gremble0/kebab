#pragma once

#include "nonstdlib/nlist.h"
#include "parser/atoms.h"

// TODO: unnecessary? combine with factor
typedef struct {
  atom_t *atom;
  list_t *arguments; // list<expression_t *> for function calls, TODO: suffix
                     // for more syntax like []
} primary_t;

primary_t *primary_parse(lexer_t *lexer);
void primary_free(primary_t *prm);
