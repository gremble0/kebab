#pragma once

#include "lexer.h"
#include "nlist.h"

#define EXPECT_TOKEN(token_kind, lexer)                                        \
  do {                                                                         \
    if (lexer->cur_token->kind != token_kind) {                                \
      err_wrong_token(#token_kind, lexer);                                     \
    }                                                                          \
  } while (0)

#define LIST_START_SIZE 5

#ifdef DEBUG
static size_t cur_indent_depth = 0;
#endif

typedef enum unary_operator_t {
  UNARY_PLUS,  // +1
  UNARY_MINUS, // -1
  UNARY_NOT,   // ~true

  UNARY_NO_OP, // Indicates missing operator
} unary_operator_t;

typedef enum binary_operator_t {
  BINARY_PLUS,  // x + y
  BINARY_MINUS, // x - y
  BINARY_MULT,  // x * y
  BINARY_DIV,   // x / y

  BINARY_NO_OP, // Indicates missing operator
} binary_operator_t;

// TODO: use somethingl like this? or just leave typing to runtime?
///////////////////
typedef enum keb_type_raw_t {
  TYPE_CHAR,
  TYPE_STRING,
  TYPE_INT,
  TYPE_LIST,
  TYPE_FN,
} keb_type_raw_t;

typedef struct keb_type_t {
  keb_type_raw_t type;
  union {
    keb_type_raw_t generic_type; // for lists e.g. list<int>
    struct {
      // for fns, e.g. fn((a : int) => int(...))
      list_t *param_types; // list<keb_type_raw_t>
      keb_type_raw_t return_type;
    };
  };
} keb_type_t;
///////////////////

typedef struct atom_t {
  enum {
    ATOM_CHAR,
    ATOM_STRING,
    ATOM_INT,
    ATOM_NAME,
    ATOM_LIST,
  } type;
  union {
    char char_value;
    const char *string_value;
    int int_value;
    const char *name;
    list_t *list_value;

    // TODO:
    // float
    // nil
    // bool
    // map?
  };
} atom_t;

typedef struct primary_t {
  atom_t *atom;
  list_t *arguments; // for function calls, TODO: suffix for more syntax like []
} primary_t;

typedef struct factor_t {
  // TODO: inline enum?
  unary_operator_t prefix; // e.g. -2, +1, ~false
  primary_t *primary;
} factor_t;

// Forward declaration because its used in fn_constructor
typedef struct constructor_t constructor_t;
typedef enum constructor_type_t {
  CONSTR_CHAR,
  CONSTR_STRING,
  CONSTR_INT,
  CONSTR_FN,
} constructor_type_t;

typedef struct char_constructor_t {
  list_t *statements; // list<statement_t>
} char_constructor_t;

typedef struct string_constructor_t {
  list_t *statements; // list<statement_t>
} string_constructor_t;

typedef struct int_constructor_t {
  list_t *statements; // list<statement_t>
} int_constructor_t;

typedef struct fn_param_t {
  const char *name;
  const char *type_name;
} fn_param_t;

typedef struct fn_constructor_t {
  list_t *params; // Only for functions, NULL for primitives
  constructor_type_t type;
  constructor_t *body;
} fn_constructor_t;

struct constructor_t {
  constructor_type_t type;
  union {
    char_constructor_t *char_constructor;
    string_constructor_t *string_constructor;
    int_constructor_t *int_constructor;
    fn_constructor_t *fn_constructor;
  };
};

typedef struct expression_t {
  list_t *factors;   // list<factor_t>
  list_t *operators; // list<binary_operator_t>
} expression_t;

// Binds a symbol to an expression
typedef struct definition_t {
  const char *name;
  int is_mutable; // flag for seeing if this definition is mutable
  constructor_type_t type;
  constructor_t *constructor;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct assignment_t {
  const char *name;
  constructor_type_t type;
  constructor_t *constructor;
} assignment_t;

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

// Entrypoint to build/use the contents of a program
typedef struct ast_t {
  list_t *statements; // list<statement_t>
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
