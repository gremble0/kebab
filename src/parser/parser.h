#pragma once

#include "lexer.h"
#include "nlist.h"

#define EXPECT_TOKEN(expected_kind, lexer)                                     \
  do {                                                                         \
    if (lexer->cur_token->kind != expected_kind) {                             \
      err_wrong_token(expected_kind, lexer);                                   \
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

// TODO: nullable types?

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
  // TODO: nil? bool map list etc.
} constructor_type_t;

// TODO: reduce duplication here

typedef struct char_constructor_t {
  list_t *statements; // list<statement_t>
} char_constructor_t;

typedef struct string_constructor_t {
  list_t *statements; // list<statement_t>
} string_constructor_t;

typedef struct int_constructor_t {
  list_t *statements; // list<statement_t>
} int_constructor_t;

// for functions that take functions as a parameter, these will have
// some more richly typed information (the parameters of that function, and
// its return type)
typedef struct fn_fn_param_t {
  list_t *param_types;            // list<constructor_type_t> ?
  constructor_type_t return_type; // ??
} fn_fn_param_t;

// TODO: implement this
typedef struct fn_list_param_t {
  constructor_type_t type; // ??
} fn_list_param_t;

typedef struct fn_param_t {
  const char *name;
  enum {
    PARAM_CHAR,
    PARAM_STRING,
    PARAM_INT,
    PARAM_FN,
    // TODO: param nil bool list etc
  } type;

  // Some parameter types can hold extra parametrized information (e.g.
  // functions, lists)
  union {
    fn_fn_param_t *fn;
    fn_list_param_t *list;
  } parametrized;
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
