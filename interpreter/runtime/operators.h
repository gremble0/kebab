#pragma once

#include "runtime/runtime.h"

rt_value_t *operator_unary_plus_eval(const rt_value_t *v);
rt_value_t *operator_unary_minus_eval(const rt_value_t *v);
rt_value_t *operator_unary_not_eval(const rt_value_t *v);

rt_value_t *operator_binary_add_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_minus_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_mult_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_div_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_lt_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_le_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_eq_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_neq_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_gt_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
rt_value_t *operator_binary_ge_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span);
