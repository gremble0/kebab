#pragma once

#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/types.h"

// Forward declaration because its used recusively
typedef struct constructor_t constructor_t;

// Primitivies are types like char, int, string, bool, etc.
typedef struct {
  list_t *body; // list<statement_t *>
  span_t span;
} constructor_primitive_t;

typedef struct {
  string_t *name;
  keb_type_t *type;
} fn_param_t;

typedef struct {
  list_t *params; // list<fn_param_t *>
  constructor_t *constr;
  span_t span;
} constructor_fn_t;

typedef struct {
  list_t *body; // list<statement_t *> where the last statement_t should be a
                // STMT_EXPRESSION
  keb_type_t *type;
  span_t span;
} constructor_list_t;

struct constructor_t {
  keb_type_t *type;
  union {
    constructor_primitive_t *primitive_constructor;
    constructor_fn_t *fn_constructor;
    constructor_list_t *list_constructor;
  };
};

constructor_t *constructor_parse(lexer_t *lexer);
void constructor_free(constructor_t *constr);
