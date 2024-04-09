#pragma once

#include "lexer.h"
#include "nlist.h"

typedef enum binary_operator_t {
  BINARY_PLUS,  // x + y
  BINARY_MINUS, // x - y
  BINARY_MULT,  // x * y
  BINARY_DIV,   // x / y

  BINARY_NO_OP, // Indicates missing operator
} binary_operator_t;

typedef struct expression_t {
  list_t *factors;   // list<factor_t>
  list_t *operators; // list<binary_operator_t>
} expression_t;

expression_t *parse_expression(lexer_t *lexer);
void expression_free(void *expr);
