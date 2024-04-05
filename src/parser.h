#include "lexer.h"
#include "nlist.h"

#define EXPECT_TOKEN(lexer, token_kind)                                        \
  do {                                                                         \
    if (lexer->cur_token->kind != token_kind) {                                \
      err_illegal_token(lexer->cur_token);                                     \
    }                                                                          \
  } while (0);

#define LIST_START_SIZE 3

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

typedef struct atom_t {
  union {
    int int_value;
    const char *string_value;
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
  unary_operator_t prefix; // e.g. -2, +1, ~false
  primary_t *primary;
} factor_t;

typedef struct string_constructor_t {
  list_t *statements; // list<statement_t>
} string_constructor_t;

typedef struct int_constructor_t {
  list_t *statements; // list<statement_t>
} int_constructor_t;

typedef struct fn_constructor_t {
  list_t *params;     // Only for functions, NULL for primitives
  void *return_type;  // TODO: type for return types?, enum kebab_type: int,
                      // string, etc. ?
  list_t *statements; // constructor body is a list of statements, constructor
                      // returns first statement that returns return_type
} fn_constructor_t;

typedef struct constructor_t {
  // TODO: enum type?
  union {
    fn_constructor_t *fn_constructor;
    int_constructor_t *int_constructor;
    string_constructor_t *string_constructor;
  };
} constructor_t;

typedef struct expr_t {
  list_t *factors;   // list<factor_t>
  list_t *operators; // list<binary_operator_t>
} expr_t;

// Binds a symbol to an expression
typedef struct definition_t {
  const char *name;
  int is_mutable; // flag for seeing if this definition is mutable
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
  union {
    definition_t *definition;
    assignment_t *assignment;
    expr_t *expr;
  };
} statement_t;

// Entrypoint to build/use the contents of a program
typedef struct ast_t {
  list_t *statements;
} ast_t;

ast_t *parse(lexer_t *lexer);
void ast_free(ast_t *ast);
