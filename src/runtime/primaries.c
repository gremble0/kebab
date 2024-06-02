#include "runtime/primaries.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/types.h"

rt_value_t *eval_func_call(list_t *arguments, fn_constructor_t *fn,
                           scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  // TODO: varargs?
  // Evaluate the parameters and put them into the local scope of the function
  // Also check their types and error if they're wrong
  for (size_t i = 0; i < arguments->size; ++i) {
    rt_value_t *arg = eval_expression(list_get(arguments, i), scope);
    fn_param_t *p = list_get(fn->params, i);

    // Verify type is correct
    type_compare(arg->type, p->type);

    scope_put(local_scope, p->name, arg);
  }

  // Evaluate the body of the function
  rt_value_t *v = eval_constructor(fn->constr, local_scope);

  scope_free(local_scope);

  return v;
}

rt_value_t *eval_primary(primary_t *prm, scope_t *scope) {
  rt_value_t *v = eval_atom(prm->atom, scope);

  if (prm->arguments != NULL) {
    // Type error - only functions are callable
    if (v->type->type != TYPE_FN)
      ASSERT(0);

    v = eval_func_call(prm->arguments, v->fn_value, scope);
  }

  return v;
}
