#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

static rt_type_t constructor_to_runtime_type(constructor_type_t ctype) {
  switch (ctype) {
  case CONSTR_CHAR:
    return RUNTIME_CHAR;

  case CONSTR_STRING:
    return RUNTIME_STRING;

  case CONSTR_INT:
    return RUNTIME_INT;

  case CONSTR_BOOL:
    return RUNTIME_BOOL;

  default:
    ASSERT(0);
  }
}

static rt_value_t *eval_constructor_body(constructor_t *constr,
                                         scope_t *scope) {
  list_t *constr_body = constr->primitive_constructor->statements;
  ASSERT(constr_body->cur_size > 0);

  for (size_t i = 0; i < constr_body->cur_size - 1; ++i)
    eval_statement(list_get(constr_body, i), scope);

  // Last statement in constructor should be an expression (this has been
  // verified in the parser)
  // @see `parse_primitive_constructor`
  statement_t *last = list_get(constr_body, constr_body->cur_size - 1);

  return eval_expression(last->expr, scope);
}

rt_value_t *eval_primitive_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  if (rtv == NULL)
    err_malloc_fail();

  scope_t *local_scope = scope_init(scope);

  rtv = eval_constructor_body(constr, local_scope);
  if (rtv->type != constructor_to_runtime_type(constr->type))
    err_type_error("", "");

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
