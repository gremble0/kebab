#pragma once

#include "constructors.h"
#include "expressions.h"

// Binds a symbol to an expression
typedef struct {
  const char *name;
  char is_mutable; // flag for seeing if this definition is mutable
  constructor_t *constructor;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct {
  const char *name;
  constructor_t *constructor;
} assignment_t;

// Sort of abstract/generic type to represent all possible statements.
// Actual statements are the types in the union of this struct.
typedef struct {
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
