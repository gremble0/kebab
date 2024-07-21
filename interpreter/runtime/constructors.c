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
#include "runtime/scope.h"
#include "runtime/statements.h"
#include "runtime/types.h"

rt_value_t *constructor_body_eval(list_t *body, scope_t *scope) {
  // Raise error here?
  ASSERT(body->size > 0);

  for (size_t i = 0; i < body->size - 1; ++i)
    statement_eval(list_get(body, i), scope);

  // Last statement in constructor should be an expression (this has been
  // verified in the parser)
  // @see `parse_primitive_constructor`
  statement_t *last = list_get(body, body->size - 1);
  ASSERT(last->type == STMT_EXPRESSION);

  return expression_eval(last->expr, scope);
}

static rt_value_t *constructor_primitive_eval(constructor_primitive_t *constr, scope_t *scope) {
  return constructor_body_eval(constr->body, scope);
}

// TODO: constructor_t instead as param?
static rt_value_t *constructor_list_eval(constructor_list_t *lc, scope_t *scope) {
  // Should return some list, however we will overwrite the type with the one
  // specified in the constructor - this is kinda scuffed, but it works
  rt_value_t *v = constructor_body_eval(lc->body, scope);
  if (v->type->kind != TYPE_LIST) {
    // If there is a type error here we will refer to the last statement in the constructor body
    // (the return statement) as the source of the error
    statement_t *last_statement = list_get(lc->body, lc->body->size - 1);
    err_opaque_type_error(TYPE_LIST, v->type->kind, last_statement->span);
  }

  v->type = malloc(sizeof(*v->type));
  v->type->kind = TYPE_LIST;
  v->type->list_type = (keb_type_list_t){.type = lc->type};

  for (size_t i = 0; i < v->list_value->size; ++i) {
    rt_value_t *cur = list_get(v->list_value, i);
    type_compare(lc->type, cur->type, lc->span);
  }

  return v;
}

rt_value_t *constructor_eval(constructor_t *constr, scope_t *scope) {
  switch (constr->type->kind) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL: {
    scope_t *local_scope = scope_init(scope);
    rt_value_t *v = constructor_primitive_eval(constr->primitive_constructor, local_scope);

    // Ensure the constructor returned its declared type
    type_compare(constr->type, v->type, constr->primitive_constructor->span);

    scope_free(local_scope);
    return v;
  }

  case TYPE_FN: {
    // TODO: kinda breaking abstraction layer
    rt_value_t *v = malloc(sizeof(*v));
    if (v == NULL)
      err_malloc_fail();

    v->fn_value = constr->fn_constructor;
    v->type = constr->type;
    return v;
  }

  case TYPE_LIST: {
    scope_t *local_scope = scope_init(scope);
    rt_value_t *v = constructor_list_eval(constr->list_constructor, local_scope);

    // TODO: kinda breaking abstraction layer
    v->type = constr->type;

    scope_free(local_scope);
    return v;
  }
  }
}
