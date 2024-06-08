#include <stdint.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/expressions.h"
#include "parser/primaries.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/primaries.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/types.h"

// TODO: rename?
static rt_value_t *func_call_eval(list_t *arguments, rt_func_t *fn,
                                  scope_t *scope) {
  scope_t *local_scope = scope_init(scope);

  // TODO: varargs?
  // Evaluate the parameters and put them into the local scope of the function
  // Also check their types and error if they're wrong
  for (size_t i = 0; i < arguments->size; ++i) {
    rt_value_t *arg = expression_eval(list_get(arguments, i), local_scope);
    fn_param_t *p = list_get(fn->params, i);

    // Verify type is correct
    type_compare(p->type, arg->type);

    scope_put(local_scope, p->name, arg);
  }

  // Evaluate the body of the function
  rt_value_t *v = constructor_eval(fn->constr, local_scope);

  scope_free(local_scope);

  return v;
}

static rt_value_t *subscription_eval(expression_t *subscription,
                                     rt_list_t *list, scope_t *scope) {
  // TODO: subscription could in the future also be for maps or other structures
  // so these assumptions may no longer be valid

  rt_value_t *subscription_v = expression_eval(subscription, scope);
  if (subscription_v->type != type_int)
    // TODO: make some error for this - can only index lists with ints
    ASSERT(0);

  if (subscription_v->int_value >= (int64_t)list->size)
    // TODO: this error is not enough when i implement proper tracing of line
    // numbers and stuff
    err_index_out_of_bounds(subscription_v->int_value, list->size);

  return list_get(list, subscription_v->int_value);
}

rt_value_t *primary_eval(primary_t *prm, scope_t *scope) {
  rt_value_t *v = atom_eval(prm->atom, scope);

  if (prm->suffixes == NULL)
    return v;

  for (size_t i = 0; i < prm->suffixes->size; ++i) {
    primary_suffix_t *psfx = list_get(prm->suffixes, i);

    // TODO: better error handling here
    switch (psfx->type) {
    case PRIMARY_SUBSCRIPTION:
      if (v->type->kind != TYPE_LIST)
        ASSERT(0);

      v = subscription_eval(psfx->subscription, v->list_value, scope);
      break;
    case PRIMARY_ARGUMENT:
      if (v->type->kind != TYPE_FN)
        ASSERT(0);

      v = func_call_eval(psfx->arguments, v->fn_value, scope);
      break;
    }
  }

  return v;
}
