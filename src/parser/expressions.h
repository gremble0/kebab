#pragma once

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"

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

  BINARY_NO_OP, // Indiciates missing operator
} binary_operator_t;

// @see https://en.wikipedia.org/wiki/Order_of_operations
// CURRENTLY UNUSED, maybe used in the future so I will keep it
static const size_t precedences[] = {
    [BINARY_PLUS] = 4, [BINARY_MINUS] = 4,

    [BINARY_MULT] = 3, [BINARY_DIV] = 3,

    [BINARY_LT] = 6,   [BINARY_LE] = 6,    [BINARY_EQ] = 6,
    [BINARY_NEQ] = 6,  [BINARY_GT] = 6,    [BINARY_GE] = 6,
};

typedef struct expression_t expression_t;

typedef struct {
  expression_t *test;
  list_t *body; // list<statement_t *>
} cond_t;

typedef struct {
  list_t *conds; // list<cond_t*>
} expression_cond_t;

typedef struct {
  list_t *factors;   // list<factor_t *>
  list_t *operators; // list<binary_operator_t *>
} expression_normal_t;

typedef struct {
  constructor_t *constr;
} expression_constructor_t;

struct expression_t {
  enum {
    EXPR_COND,
    EXPR_NORMAL,
    EXPR_CONSTRUCTOR,
  } type;
  union {
    expression_normal_t *normal;
    expression_cond_t *cond;
    expression_constructor_t *constr;
  };
};

expression_t *expression_parse(lexer_t *lexer);
void expression_free(expression_t *expr);
