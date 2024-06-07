#pragma once

#include "parser/factors.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *factor_eval(factor_t *fac, scope_t *scope);
