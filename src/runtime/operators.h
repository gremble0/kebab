#include "runtime/runtime.h"

rt_value_t *eval_operator_unary_plus(rt_value_t *v);
rt_value_t *eval_operator_unary_minus(rt_value_t *v);
rt_value_t *eval_operator_unary_not(rt_value_t *v);

rt_value_t *eval_operator_binary_add(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_minus(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_mult(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_div(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_lt(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_le(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_eq(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_neq(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_gt(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *eval_operator_binary_ge(rt_value_t *lhs, rt_value_t *rhs);
