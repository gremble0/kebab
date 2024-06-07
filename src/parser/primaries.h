#pragma once

#include "nonstdlib/nlist.h"
#include "parser/atoms.h"
#include "parser/expressions.h"

typedef struct {
  enum {
    PRIMARY_SUBSCRIPTION,
    PRIMARY_ARGUMENT,
  } type;
  union {
    list_t *arguments; // list<expression_t *>

    // TODO: allow subscribing nested lists like this: [0, 1, 2] which would be
    // equivalent to [0][1][2]
    expression_t *subscription;
  };
} primary_suffix_t;

// TODO: unnecessary? combine with factor
typedef struct {
  atom_t *atom;
  list_t *suffixes; // list<primary_suffix_t *> for function calls, TODO: suffix
                    // for more syntax like []
} primary_t;

primary_t *primary_parse(lexer_t *lexer);
void primary_free(primary_t *prm);
