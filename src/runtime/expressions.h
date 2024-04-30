#include "parser/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_expression(expression_t *expr, scope_t *scope);
