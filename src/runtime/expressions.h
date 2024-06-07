#pragma once

#include "parser/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *expression_eval(expression_t *expr, scope_t *scope);
