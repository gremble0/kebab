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
    expression_t *subscription;
  };
} primary_suffix_t;

typedef struct {
  atom_t *atom;
  list_t *suffixes; // list<primary_suffix_t *>
  span_t span;
} primary_t;

primary_t *primary_parse(lexer_t *lexer);
void primary_free(primary_t *prm);
