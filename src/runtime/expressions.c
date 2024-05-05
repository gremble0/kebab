#include "parser/expressions.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "runtime/expressions.h"
#include "runtime/factors.h"
#include "runtime/runtime.h"

static rt_value_t *eval_expression_cond(expression_cond_t *excd,
                                        scope_t *scope) {
  return NULL;
}

static rt_value_t *eval_expression_normal(expression_normal_t *exnm,
                                          scope_t *scope) {
  // Evaluate the first factor then apply all operations between the first
  // factor and the other factors
  rt_value_t *evaluated = eval_factor(list_get(exnm->factors, 0), scope);
  for (size_t i = 0; i < exnm->operators->cur_size; ++i) {
    binary_operator_t *bo = list_get(exnm->operators, i);
    rt_value_t *next_evaluated =
        eval_factor(list_get(exnm->factors, i + 1), scope);

    switch (*bo) {
    case BINARY_PLUS:
      evaluated = eval_add(evaluated, next_evaluated);
      break;

    case BINARY_MINUS:
      evaluated = eval_minus(evaluated, next_evaluated);
      break;

    case BINARY_MULT:
      evaluated = eval_mult(evaluated, next_evaluated);
      break;

    case BINARY_DIV:
      evaluated = eval_div(evaluated, next_evaluated);
      break;

    case BINARY_LT:
      evaluated = eval_lt(evaluated, next_evaluated);
      break;

    case BINARY_LE:
      evaluated = eval_le(evaluated, next_evaluated);
      break;

    case BINARY_EQ:
      evaluated = eval_eq(evaluated, next_evaluated);
      break;

    case BINARY_NEQ:
      evaluated = eval_neq(evaluated, next_evaluated);
      break;

    case BINARY_GT:
      evaluated = eval_gt(evaluated, next_evaluated);
      break;

    case BINARY_GE:
      evaluated = eval_ge(evaluated, next_evaluated);
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
