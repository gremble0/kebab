#pragma once

#include "parser/constructors.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *constructor_eval(constructor_t *constr, scope_t *scope);
rt_value_t *constructor_body_eval(list_t *body, scope_t *scope);
