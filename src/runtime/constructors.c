#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/types.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

// TODO: take fn_constructor_t, primitive_constructor_t, etc. as param, BUT this
// is hard since primitive_constructor_t doesnt store its type

static rt_value_t *eval_constructor_body(primitive_constructor_t *constr,
                                         scope_t *scope) {
  list_t *constr_body = constr->stmts;
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

static rt_value_t *eval_primitive_constructor(primitive_constructor_t *constr,
                                              scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  rt_value_t *v = eval_constructor_body(constr, local_scope);

  scope_free(local_scope);

  return v;
}

static rt_value_t *eval_fn_constructor(fn_constructor_t *constr) {
  rt_value_t *v = malloc(sizeof(*v));
  v->type = TYPE_FN;
  v->fn_value = constr;

  return v;
}

static rt_value_t *eval_list_constructor(list_constructor_t *constr,
                                         scope_t *scope) {
  rt_value_t *v = malloc(sizeof(*v));

  return v;
}

rt_value_t *eval_constructor(constructor_t *constr, scope_t *scope) {
  switch (constr->type) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL: {
    rt_value_t *v =
        eval_primitive_constructor(constr->primitive_constructor, scope);
    if (constr->type != v->type)
      err_type_error(type_kind_map[constr->type], type_kind_map[v->type]);
    return v;
  }
  case TYPE_FN:
    return eval_fn_constructor(constr->fn_constructor);
  case TYPE_LIST:
    return eval_list_constructor(constr->list_constructor, scope);
  }
}
