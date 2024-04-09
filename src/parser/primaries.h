#pragma once

#include "atoms.h"
#include "nlist.h"

// TODO: unnecessary? combine with factor
typedef struct primary_t {
  atom_t *atom;
  list_t *arguments; // for function calls, TODO: suffix for more syntax like []
} primary_t;

primary_t *parse_primary(lexer_t *lexer);
void primary_free(primary_t *prm);
