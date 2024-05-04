#include "runtime/expressions.h"
#include "parser/expressions.h"
#include "runtime/runtime.h"

static rt_value_t *eval_expression_cond(expression_cond_t *excd,
                                        scope_t *scope) {}

static rt_value_t *eval_expression_normal(expression_normal_t *exnm,
                                          scope_t *scope) {}

rt_value_t *eval_expression(expression_t *expr, scope_t *scope) {
  switch (expr->type) {
  case EXPR_COND:
    return eval_expression_cond(expr->cond, scope);
  case EXPR_NORMAL:
    return eval_expression_normal(expr->normal, scope);
  }
}
