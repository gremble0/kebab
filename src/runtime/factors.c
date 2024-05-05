#include "runtime/factors.h"
#include "parser/factors.h"
#include "runtime/operators.h"
#include "runtime/primaries.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_factor(factor_t *fac, scope_t *scope) {
  rt_value_t *v = eval_primary(fac->primary, scope);

  // TODO: THIS WILL RESULT IN MEMORY LEAK
  switch (fac->prefix) {
  case UNARY_PLUS:
    v = eval_operator_unary_plus(v);
    break;
  case UNARY_MINUS:
    v = eval_operator_unary_minus(v);
    break;
  case UNARY_NOT:
    v = eval_operator_unary_not(v);
    break;
  case UNARY_NO_OP:
    break;
  }

  return v;
}
