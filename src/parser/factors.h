#pragma once

#include "primaries.h"

typedef enum unary_operator_t {
  UNARY_PLUS,  // +1
  UNARY_MINUS, // -1
  UNARY_NOT,   // ~true

  UNARY_NO_OP, // Indicates missing operator
} unary_operator_t;

// TODO: nullable types - would be in factor probably?
typedef struct factor_t {
  // TODO: inline enum?
  unary_operator_t prefix; // e.g. -2, +1, ~false
  primary_t *primary;
} factor_t;

factor_t *parse_factor(lexer_t *lexer);
void factor_free(void *fac);