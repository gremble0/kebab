#pragma once

#include "lexer.h"
#include "nlist.h"

typedef enum {
  // Maths
  BINARY_PLUS,  // x + y
  BINARY_MINUS, // x - y
  BINARY_MULT,  // x * y
  BINARY_DIV,   // x / y

  // Comparisons
  BINARY_LT,  // x < y
  BINARY_LE,  // x <= y
  BINARY_EQ,  // x == y
  BINARY_NEQ, // x ~= y
  BINARY_GT,  // x > y
  BINARY_GE,  // x >= y
} binary_operator_t;

typedef struct {
  list_t *factors;   // list<factor_t *>
  list_t *operators; // list<binary_operator_t *>
} expression_t;

expression_t *parse_expression(lexer_t *lexer);
void expression_free(expression_t *expr);
