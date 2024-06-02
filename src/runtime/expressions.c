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

static rt_value_t *eval_expression_cond(expression_cond_t *excd,
                                        scope_t *scope) {
  // - 1 because last cond is the else branch which doesnt have a test (NULL)
  for (size_t i = 0; i < excd->conds->size - 1; ++i) {
    cond_t *cond = list_get(excd->conds, i);
    rt_value_t *tested = eval_expression(cond->test, scope);
    // TODO: fill in this (probably new type of error)
    if (tested->type != type_bool)
      err_type_error(type_kind_map[TYPE_BOOL],
                     type_kind_map[tested->type->type]);

    if (tested->bool_value)
      return eval_constructor_body(cond->body, scope);
  }

  // Take else branch if none of the tests were true
  cond_t *else_cond = list_get(excd->conds, excd->conds->size - 1);
  return eval_constructor_body(else_cond->body, scope);
}

static rt_value_t *eval_expression_normal(expression_normal_t *exnm,
                                          scope_t *scope) {
  // Evaluate the first factor then apply all operations between the first
  // factor and the other factors
  rt_value_t *evaluated = eval_factor(list_get(exnm->factors, 0), scope);
  for (size_t i = 0; i < exnm->operators->size; ++i) {
    binary_operator_t *bo = list_get(exnm->operators, i);
    rt_value_t *next_evaluated =
        eval_factor(list_get(exnm->factors, i + 1), scope);

    switch (*bo) {
    case BINARY_PLUS:
      evaluated = eval_operator_binary_add(evaluated, next_evaluated);
      break;

    case BINARY_MINUS:
      evaluated = eval_operator_binary_minus(evaluated, next_evaluated);
      break;

    case BINARY_MULT:
      evaluated = eval_operator_binary_mult(evaluated, next_evaluated);
      break;

    case BINARY_DIV:
      evaluated = eval_operator_binary_div(evaluated, next_evaluated);
      break;

    case BINARY_LT:
      evaluated = eval_operator_binary_lt(evaluated, next_evaluated);
      break;

    case BINARY_LE:
      evaluated = eval_operator_binary_le(evaluated, next_evaluated);
      break;

    case BINARY_EQ:
      evaluated = eval_operator_binary_eq(evaluated, next_evaluated);
      break;

    case BINARY_NEQ:
      evaluated = eval_operator_binary_neq(evaluated, next_evaluated);
      break;

    case BINARY_GT:
      evaluated = eval_operator_binary_gt(evaluated, next_evaluated);
      break;

    case BINARY_GE:
      evaluated = eval_operator_binary_ge(evaluated, next_evaluated);
      break;

    case BINARY_NO_OP:
      // Unreachable
      ASSERT(0);
    }
  }

  return evaluated;
}

rt_value_t *eval_expression(expression_t *expr, scope_t *scope) {
  switch (expr->type) {
  case EXPR_COND:
    return eval_expression_cond(expr->cond, scope);
  case EXPR_NORMAL:
    return eval_expression_normal(expr->normal, scope);
  }
}
