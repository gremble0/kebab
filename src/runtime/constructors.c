#include <stdio.h>
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

rt_value_t *eval_constructor_body(list_t *body, scope_t *scope) {
  // Raise error here?
  ASSERT(body->size > 0);

  for (size_t i = 0; i < body->size - 1; ++i)
    eval_statement(list_get(body, i), scope);

  // Last statement in constructor should be an expression (this has been
  // verified in the parser)
  // @see `parse_primitive_constructor`
  statement_t *last = list_get(body, body->size - 1);
  ASSERT(last->type == STMT_EXPRESSION);

  return eval_expression(last->expr, scope);
}

static rt_value_t *eval_primitive_constructor(primitive_constructor_t *constr,
                                              scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  rt_value_t *v = eval_constructor_body(constr->body, local_scope);

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
  // Body should return a list
  rt_value_t *v = eval_constructor_body(constr->body, scope);
  if (v->type != TYPE_LIST)
    err_type_error(type_kind_map[TYPE_LIST], type_kind_map[v->type]);

  // TODO: THIS IS NOT GOOD ENOUGH FOR COMPOSITE TYPES
  for (size_t i = 0; i < v->list_value->size; ++i) {
    rt_value_t *cur = list_get(v->list_value, i);
    if (cur->type != constr->type->type)
      err_list_type_error(type_kind_map[constr->type->type],
                          type_kind_map[cur->type]);
  }

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
