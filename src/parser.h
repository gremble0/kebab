#include "lexer.h"
#include "list.h"

typedef enum operator_t {
  OP_PLUS,
  OP_MINUS,
  OP_MULT,
  OP_DIV,
} operator_t;

typedef struct atom_t {
  union {
    int int_value;
    const char *string_value;
    list_t *list_value;
    // float
    // nil
    // bool
    // map
  };
  // TODO: structs for the types that allow pre/suf-fix ?
  operator_t prefix; // e.g. -2
  operator_t suffix; // e.g 2 - ...
} atom_t;

typedef struct string_constructor_t {
  list_t *statements;
} string_constructor_t;

typedef struct int_constructor_t {
  list_t *statements;
} int_constructor_t;

typedef struct fn_constructor_t {
  list_t *params;     // Only for functions, NULL for primitives
  void *return_type;  // TODO: type for return types?
  list_t *statements; // constructor body is a list of statements, constructor
                      // returns first statement that returns return_type
} fn_constructor_t;

typedef struct constructor_t {
  union {
    fn_constructor_t *fn_constructor;
    int_constructor_t *int_constructor;
    string_constructor_t *string_constructor;
  };
} constructor_t;

typedef struct expr_t {
  list_t *atoms;
} expr_t;

typedef struct fn_call_t {
  const char *fn_name; // TODO: fn type? maybe when evaling parsed code
  list_t *arguments;
} fn_call_t;

// Binds a symbol to an expression
typedef struct definition_t {
  const char *symbol;
  constructor_t *constructor;
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
    expr_t *expr;
  };
} statement_t;

// Entrypoint to build/use the contents of a program
typedef struct ast_t {
  list_t *statements;
} ast_t;

ast_t *parse_lexer(lexer_t *lexer);
void parse_free(ast_t *ast);
