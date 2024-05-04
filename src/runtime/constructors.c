#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

// TODO: take fn_constructor_t, primitive_constructor_t, etc. as param, BUT this
// is hard since primitive_constructor_t doesnt store its type

static rt_value_t *eval_constructor_body(constructor_t *constr,
                                         scope_t *scope) {
  list_t *constr_body = constr->primitive_constructor->stmts;
  ASSERT(constr_body->cur_size > 0);

  for (size_t i = 0; i < constr_body->cur_size - 1; ++i)
    eval_statement(list_get(constr_body, i), scope);

  // Last statement in constructor should be an expression (this has been
  // verified in the parser)
  // @see `parse_primitive_constructor`
  statement_t *last = list_get(constr_body, constr_body->cur_size - 1);
  ASSERT(last->type == STMT_EXPRESSION);

  return eval_expression(last->expr, scope);
}

rt_value_t *eval_primitive_constructor(constructor_t *constr, scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  rt_value_t *rtv = eval_constructor_body(constr, local_scope);
  if (constr->type != rtv->type)
    err_type_error(type_kind_map[constr->type], type_kind_map[rtv->type]);

  scope_free(local_scope);

  return rtv;
}

rt_value_t *eval_fn_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == TYPE_FN);

  rt_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}

rt_value_t *eval_list_constructor(constructor_t *constr, scope_t *scope) {
  ASSERT(constr->type == TYPE_LIST);

  rt_value_t *rtv = malloc(sizeof(*rtv));

  return rtv;
}
