#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "runtime/constructors.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

static void eval_constructor_body(constructor_t *constr, scope_t *scope) {
  list_t *constr_body = constr->primitive_constructor->statements;
  for (size_t i = 0; i < constr_body->cur_size; ++i)
    eval_statement(list_get(constr_body, i), scope);
}

rt_value_t *eval_char_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  scope_t *local_scope = scope_init(scope);

  eval_constructor_body(constr, local_scope);

  // Get return type of body

  scope_free(local_scope);

  return rtv;
}

rt_value_t *eval_fn_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == CONSTR_FN);

  rt_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}

rt_value_t *eval_list_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == CONSTR_LIST);

  rt_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}
