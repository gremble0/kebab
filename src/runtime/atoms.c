#include <stdlib.h>

#include "nonstdlib/nlist.h"
#include "parser/atoms.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "utils/utils.h"

rt_value_t *atom_eval(atom_t *atom, scope_t *scope) {
  switch (atom->type) {
  case ATOM_CHAR: {
    rt_value_t *v = malloc(sizeof(*v));
    v->char_value = atom->char_value;
    v->type = type_char;
    return v;
  }

  case ATOM_STRING: {
    rt_value_t *v = malloc(sizeof(*v));
    v->string_value = atom->string_value;
    v->type = type_string;
    return v;
  }

  case ATOM_INT: {
    rt_value_t *v = malloc(sizeof(*v));
    v->int_value = atom->int_value;
    v->type = type_int;
    return v;
  }

  case ATOM_BOOL: {
    rt_value_t *v = malloc(sizeof(*v));
    v->bool_value = atom->bool_value;
    v->type = type_bool;
    return v;
  }

  case ATOM_NAME: {
    // Look up value in scope and return it, errors if variable doesnt exist
    rt_value_t *v = scope_get(scope, atom->name_value);
    if (v == NULL)
      err_name_error(atom);

    return v;
  }

  case ATOM_INNER_EXPR:
    return expression_eval(atom->inner_expr_value, scope);

  case ATOM_LIST: {
    rt_value_t *v = malloc(sizeof(*v));
    v->list_value = list_init(LIST_START_SIZE);
    v->type = type_unparametrized_list;

    for (size_t i = 0; i < atom->list_value->size; ++i)
      list_push_back(v->list_value,
                     expression_eval(list_get(atom->list_value, i), scope));

    return v;
  }
  }
}
