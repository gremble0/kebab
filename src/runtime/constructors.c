#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "runtime/constructors.h"

// TODO: this is not sufficient, need to have separate types for each
// constructor
runtime_value_t *eval_primitive_constructor(constructor_t *constr,
                                            scope_t *scope) {
  ASSERT(constr->type == CONSTR_PRIMITIVE);

  runtime_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}

runtime_value_t *eval_fn_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == CONSTR_FN);

  runtime_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}

runtime_value_t *eval_list_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == CONSTR_LIST);

  runtime_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}
