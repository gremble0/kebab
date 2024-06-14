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
} primitive_constructor_t;

typedef struct {
  string_t *name;
  keb_type_t *type;
} fn_param_t;

typedef struct {
  list_t *params; // list<fn_param_t *>
  constructor_t *constr;
  span_t span;
} fn_constructor_t;

typedef struct {
  list_t *body; // list<statement_t *> where the last statement_t should be a
                // STMT_EXPRESSION
  keb_type_t *type;
  span_t span;
} list_constructor_t;

struct constructor_t {
  keb_type_t *type;
  union {
    primitive_constructor_t *primitive_constructor;
    fn_constructor_t *fn_constructor;
    list_constructor_t *list_constructor;
  };
};

constructor_t *constructor_parse(lexer_t *lexer);
void constructor_free(constructor_t *constr);
