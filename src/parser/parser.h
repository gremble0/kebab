#pragma once

// TODO: abbreviate references to generic types e.g. definition -> def?

#include "lexer.h"
#include "nlist.h"

#define EXPECT_TOKEN(expected_kind, lexer)                                     \
  if (lexer->cur_token->kind != expected_kind) {                               \
    err_wrong_token(expected_kind, lexer);                                     \
  }

#define SKIP_TOKEN(expected_kind, lexer)                                       \
  do {                                                                         \
    EXPECT_TOKEN(expected_kind, lexer);                                        \
    lexer_advance(lexer);                                                      \
  } while (0)

#ifdef DEBUG
static size_t cur_indent_depth = 0;
#define START_PARSING(section) start_parsing(section)
#define FINISH_PARSING(section) finish_parsing(section)
#define START_AND_FINISH_PARSING(section) start_and_finish_parsing(section)
#else
#define START_PARSING(section)
#define FINISH_PARSING(section)
#define START_AND_FINISH_PARSING(section)
#endif

#define LIST_START_SIZE 5

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

// TODO: merge this with existing constructor type things
typedef struct keb_type_t keb_type_t;

typedef struct keb_type_fn_t {
  list_t *param_types; // list<keb_type_t>
  keb_type_t *return_type;
} keb_type_fn_t;

typedef struct keb_type_list_t {
  keb_type_t *type;
} keb_type_list_t;

struct keb_type_t {
  enum {
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_INT,
    TYPE_LIST,
    TYPE_FN,
  } type;
  union {
    keb_type_fn_t *fn;
    keb_type_list_t *list;
  };
};

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

typedef struct fn_param_t {
  const char *name;
  keb_type_t *type;
} fn_param_t;

typedef struct fn_constructor_t {
  list_t *params; // list<fn_param_t>
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
  char is_mutable; // flag for seeing if this definition is mutable
  constructor_t *constructor;
} definition_t;

// Same as a definition, but is only allowed for mutable bindings
typedef struct assignment_t {
  const char *name;
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
