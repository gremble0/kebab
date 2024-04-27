#pragma once

#include "nonstdlib/nlist.h"
#include "parser/types.h"

// Forward declaration because its used recusively
typedef struct constructor_struct constructor_t;

typedef enum {
  CONSTR_CHAR,
  CONSTR_STRING,
  CONSTR_INT,
  CONSTR_BOOL,
  CONSTR_FN,
  CONSTR_LIST,
} constructor_type_t;

// Primitivies are types like char, int, string, bool, etc.
typedef struct {
  list_t *statements; // list<statement_t *>
} primitive_constructor_t;

typedef struct {
  char *name;
  keb_type_t *type;
} fn_param_t;

typedef struct {
  list_t *params; // list<fn_param_t *>
  constructor_t *body;
} fn_constructor_t;

typedef struct {
  list_t *stmts; // list<statement_t *>
  keb_type_t *type;
} list_constructor_t;

struct constructor_struct {
  constructor_type_t type;
  union {
    primitive_constructor_t *primitive_constructor;
    fn_constructor_t *fn_constructor;
    list_constructor_t *list_constructor;
  };
};

constructor_t *parse_constructor(lexer_t *lexer);
void constructor_free(constructor_t *constr);
