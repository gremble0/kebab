#include "runtime/runtime.h"

rt_value_t *operator_unary_plus_eval(rt_value_t *v);
rt_value_t *operator_unary_minus_eval(rt_value_t *v);
rt_value_t *operator_unary_not_eval(rt_value_t *v);

rt_value_t *operator_binary_add_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_minus_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_mult_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_div_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_lt_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_le_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_eq_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_neq_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_gt_eval(rt_value_t *lhs, rt_value_t *rhs);
rt_value_t *operator_binary_ge_eval(rt_value_t *lhs, rt_value_t *rhs);
