#include "list.h"

typedef struct expr_t {
} expr_t;

typedef struct fn_call_t {
  const char *fn_name;
  list_t *arguments;
} fn_call_t;

typedef struct fn_def_t {
  list_t *params;
  void *return_type;  // TODO: type for return types?
  list_t *statements; // function body is a list of statements
} fn_def_t;

// Binds a symbol to an expression
typedef struct definition_t {
  const char *symbol;
  expr_t *expr;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct assignment_t {
  const char *symbol;
  expr_t *expr;
} assignment_t;

// Sort of abstract/generic type to represent all possible statements.
// Actual statements are the types in the union of this struct.
typedef struct statement_t {
  union {
    definition_t *definition;
    assignment_t *assignment;
    fn_call_t *fn_call;
  };
} statement_t;

typedef struct ast_t {
  list_t *statements;
} ast_t;
