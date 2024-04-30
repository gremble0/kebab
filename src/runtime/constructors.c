#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

static rt_value_t *eval_primitive_constructor_body(constructor_t *constr,
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

rt_value_t *eval_char_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  scope_t *local_scope = scope_init(scope);

  rtv = eval_primitive_constructor_body(constr, local_scope);
  if (rtv->type != RUNTIME_CHAR)
    err_type_error("char", "");

  scope_free(local_scope);

  return rtv;
}

rt_value_t *eval_string_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  scope_t *local_scope = scope_init(scope);

  eval_primitive_constructor_body(constr, local_scope);

  // Get return type of body

  scope_free(local_scope);

  return rtv;
}

rt_value_t *eval_int_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  scope_t *local_scope = scope_init(scope);

  eval_primitive_constructor_body(constr, local_scope);

  // Get return type of body

  scope_free(local_scope);

  return rtv;
}

rt_value_t *eval_bool_constructor(constructor_t *constr, scope_t *scope) {
  rt_value_t *rtv = malloc(sizeof(*rtv));
  scope_t *local_scope = scope_init(scope);

  eval_primitive_constructor_body(constr, local_scope);

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
