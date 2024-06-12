#pragma once

#include "parser/primaries.h"

typedef enum {
  UNARY_PLUS,  // +1
  UNARY_MINUS, // -1
  UNARY_NOT,   // ~true

  UNARY_NO_OP, // Indicates missing operator
} unary_operator_t;

// TODO: nullable types - would be in factor probably?
typedef struct {
  unary_operator_t prefix; // e.g. -2, +1, ~false
  primary_t *primary;
  span_t span;
} factor_t;

factor_t *factor_parse(lexer_t *lexer);
void factor_free(factor_t *fac);
