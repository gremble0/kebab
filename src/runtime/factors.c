#include "runtime/factors.h"
#include "parser/factors.h"
#include "runtime/operators.h"
#include "runtime/primaries.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *factor_eval(factor_t *fac, scope_t *scope) {
  rt_value_t *v = primary_eval(fac->primary, scope);

  // TODO: THIS WILL RESULT IN MEMORY LEAK
  switch (fac->prefix) {
  case UNARY_PLUS:
    v = operator_unary_plus_eval(v);
    break;
  case UNARY_MINUS:
    v = operator_unary_minus_eval(v);
    break;
  case UNARY_NOT:
    v = operator_unary_not_eval(v);
    break;
  case UNARY_NO_OP:
    break;
  }

  return v;
}
