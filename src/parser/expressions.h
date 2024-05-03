#pragma once

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"

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
  list_t *tests;  // list<expression_t *>
  list_t *bodies; // list<list<statement_t *>*> where the last statement_t in
                  // each list should be a STMT_EXPRESSION
} expr_cond_t;

typedef struct {
  list_t *factors;   // list<factor_t *>
  list_t *operators; // list<binary_operator_t *>
} expr_normal_t;

typedef struct {
  enum {
    EXPR_COND,
    EXPR_NORMAL,
  } type;
  union {
    expr_normal_t *normal;
    expr_cond_t *cond;
  };
} expression_t;

expression_t *parse_expression(lexer_t *lexer);
expression_t *parse_inner_expression(lexer_t *lexer);
void expression_free(expression_t *expr);
