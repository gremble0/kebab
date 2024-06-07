#include "runtime/primaries.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/primaries.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/types.h"

// static rt_value_t *eval_primary_suffix(primary_suffix_t

static rt_value_t *eval_func_call(list_t *arguments, fn_constructor_t *fn,
                                  scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  // TODO: varargs?
  // Evaluate the parameters and put them into the local scope of the function
  // Also check their types and error if they're wrong
  for (size_t i = 0; i < arguments->size; ++i) {
    rt_value_t *arg = eval_expression(list_get(arguments, i), scope);
    fn_param_t *p = list_get(fn->params, i);

    // Verify type is correct
    type_compare(p->type, arg->type);

    scope_put(local_scope, p->name, arg);
  }

  // Evaluate the body of the function
  rt_value_t *v = eval_constructor(fn->constr, local_scope);

  scope_free(local_scope);

  return v;
}

rt_value_t *eval_primary(primary_t *prm, scope_t *scope) {
  rt_value_t *v = eval_atom(prm->atom, scope);

  if (prm->suffixes == NULL)
    return v;

  for (size_t i = 0; i < prm->suffixes->size; ++i) {
    primary_suffix_t *psfx = list_get(prm->suffixes, i);
    switch (psfx->type) {
    case PRIMARY_SUBSCRIPTION:
    case PRIMARY_ARGUMENT:
      break;
    }
  }
  if (prm->suffixes->) {
    // Type error - only functions are callable
    if (v->type->type != TYPE_FN)
      ASSERT(0);

    v = eval_func_call(prm->arguments, v->fn_value, scope);
  }

  return v;
}
