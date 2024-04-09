#pragma once

#include "constructors.h"

// Binds a symbol to an expression
typedef struct definition_t {
  const char *name;
  char is_mutable; // flag for seeing if this definition is mutable
  constructor_t *constructor;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct assignment_t {
  const char *name;
  constructor_t *constructor;
} assignment_t;

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

// Sort of abstract/generic type to represent all possible statements.
// Actual statements are the types in the union of this struct.
typedef struct statement_t {
  enum {
    STMT_DEFINITION,
    STMT_ASSIGNMENT,
    STMT_EXPRESSION,
  } type;
  union {
    definition_t *definition;
    assignment_t *assignment;
    expression_t *expr;
  };
} statement_t;

statement_t *parse_statement(lexer_t *lexer);
expression_t *parse_expression(lexer_t *lexer);

void statement_free(void *stmt);
void expression_free(void *expr);
