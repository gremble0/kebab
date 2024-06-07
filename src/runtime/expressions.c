#include "parser/expressions.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/types.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/factors.h"
#include "runtime/operators.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

static rt_value_t *expression_cond_eval(expression_cond_t *excd,
                                        scope_t *scope) {
  // - 1 because last cond is the else branch which doesnt have a test (NULL)
  for (size_t i = 0; i < excd->conds->size - 1; ++i) {
    cond_t *cond = list_get(excd->conds, i);
    rt_value_t *tested = expression_eval(cond->test, scope);
    // TODO: fill in this (probably new type of error)
    if (tested->type != type_bool)
      err_type_error(type_kind_map[TYPE_BOOL],
                     type_kind_map[tested->type->type]);

    if (tested->bool_value)
      return constructor_body_eval(cond->body, scope);
  }

  // Take else branch if none of the tests were true
  cond_t *else_cond = list_get(excd->conds, excd->conds->size - 1);
  return constructor_body_eval(else_cond->body, scope);
}

static rt_value_t *expression_normal_eval(expression_normal_t *exnm,
                                          scope_t *scope) {
  // Evaluate the first factor then apply all operations between the first
  // factor and the other factors
  rt_value_t *evaluated = factor_eval(list_get(exnm->factors, 0), scope);
  for (size_t i = 0; i < exnm->operators->size; ++i) {
    binary_operator_t *bo = list_get(exnm->operators, i);
    // This should be freed after performing binary operator methinks
    rt_value_t *next_evaluated =
        factor_eval(list_get(exnm->factors, i + 1), scope);

    switch (*bo) {
    case BINARY_PLUS:
      evaluated = operator_binary_add_eval(evaluated, next_evaluated);
      break;

    case BINARY_MINUS:
      evaluated = operator_binary_minus_eval(evaluated, next_evaluated);
      break;

    case BINARY_MULT:
      evaluated = operator_binary_mult_eval(evaluated, next_evaluated);
      break;

    case BINARY_DIV:
      evaluated = operator_binary_div_eval(evaluated, next_evaluated);
      break;

    case BINARY_LT:
      evaluated = operator_binary_lt_eval(evaluated, next_evaluated);
      break;

    case BINARY_LE:
      evaluated = operator_binary_le_eval(evaluated, next_evaluated);
      break;

    case BINARY_EQ:
      evaluated = operator_binary_eq_eval(evaluated, next_evaluated);
      break;

    case BINARY_NEQ:
      evaluated = operator_binary_neq_eval(evaluated, next_evaluated);
      break;

    case BINARY_GT:
      evaluated = operator_binary_gt_eval(evaluated, next_evaluated);
      break;

    case BINARY_GE:
      evaluated = operator_binary_ge_eval(evaluated, next_evaluated);
      break;

    case BINARY_NO_OP:
      // Unreachable
      ASSERT(0);
    }
  }

  return evaluated;
}

rt_value_t *expression_constructor_eval(expression_constructor_t *exco,
                                        scope_t *scope) {
  return constructor_eval(exco->constr, scope);
}

rt_value_t *expression_eval(expression_t *expr, scope_t *scope) {
  switch (expr->type) {
  case EXPR_COND:
    return expression_cond_eval(expr->cond, scope);
  case EXPR_NORMAL:
    return expression_normal_eval(expr->normal, scope);
  case EXPR_CONSTRUCTOR:
    return expression_constructor_eval(expr->constr, scope);
  }
}
