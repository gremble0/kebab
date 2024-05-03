#pragma once

#include "parser/constructors.h"
#include "parser/expressions.h"

// Binds a symbol to an expression
typedef struct {
  char *name;
  char is_mutable; // flag for seeing if this definition is mutable
  constructor_t *constructor;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct {
  char *name;
  constructor_t *constructor;
} assignment_t;

typedef struct {
  list_t *tests;  // list<expression_t *>
  list_t *bodies; // list<statement_t *>
} cond_t;

// Sort of abstract/generic type to represent all possible statements.
// Actual statements are the types in the union of this struct.
typedef struct {
  enum {
    STMT_DEFINITION,
    STMT_ASSIGNMENT,
    STMT_COND,
    STMT_EXPRESSION,
  } type;
  union {
    definition_t *definition;
    assignment_t *assignment;
    expression_t *expr;
    cond_t *cond;
  };
} statement_t;

statement_t *parse_statement(lexer_t *lexer);
void statement_free(statement_t *stmt);
