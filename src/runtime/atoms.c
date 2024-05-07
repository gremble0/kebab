#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/atoms.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_atom(atom_t *atom, scope_t *scope) {
  switch (atom->type) {
  case ATOM_CHAR: {
    rt_value_t *v = malloc(sizeof(*v));
    v->char_value = atom->char_value;
    v->type = TYPE_CHAR;
    return v;
  }

  case ATOM_STRING: {
    rt_value_t *v = malloc(sizeof(*v));
    v->string_value = atom->string_value;
    v->type = TYPE_STRING;
    return v;
  }

  case ATOM_INT: {
    rt_value_t *v = malloc(sizeof(*v));
    v->int_value = atom->int_value;
    v->type = TYPE_INT;
    return v;
  }

  case ATOM_BOOL: {
    rt_value_t *v = malloc(sizeof(*v));
    v->bool_value = atom->bool_value;
    v->type = TYPE_BOOL;
    return v;
  }

  case ATOM_NAME: {
    // Look up value in scope and return it, errors if variable doesnt exist
    rt_value_t *v = scope_get(scope, atom->name_value);
    if (v == NULL)
      ASSERT(0);

    return v;
  }

  case ATOM_INNER_EXPR:
    return eval_expression(atom->inner_expr_value, scope);

  case ATOM_LIST: {
    rt_value_t *v = malloc(sizeof(*v));
    v->list_value = atom->list_value;
    v->type = TYPE_LIST;
    return v;
  }
  }
}
