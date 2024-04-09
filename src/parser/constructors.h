#pragma once

#include "nlist.h"
#include "types.h"

// Forward declaration because its used in fn_constructor
typedef struct constructor_t constructor_t;

typedef enum constructor_type_t {
  CONSTR_PRIMITIVE,
  CONSTR_FN,
  CONSTR_LIST,
  // TODO: nil? bool map list etc.
} constructor_type_t;

// TODO: reduce duplication here

// Primitivies are types like char, int, string, bool, etc.
typedef struct primitive_constructor_t {
  list_t *statements; // list<statement_t>
} primitive_constructor_t;

typedef struct fn_param_t {
  const char *name;
  keb_type_t *type;
} fn_param_t;

typedef struct fn_constructor_t {
  list_t *params; // list<fn_param_t>
  constructor_t *body;
} fn_constructor_t;

typedef struct {
  list_t *stmts; // list<statement_t>
  keb_type_t *type;
} list_constructor_t;

struct constructor_t {
  constructor_type_t type;
  // TODO: unnecessary indirection here
  union {
    primitive_constructor_t *primitive_constructor;
    fn_constructor_t *fn_constructor;
    list_constructor_t *list_constructor;
  };
};

constructor_t *parse_constructor(lexer_t *lexer);
void constructor_free(constructor_t *constr);
