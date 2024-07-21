#pragma once

#include "parser/primaries.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *primary_eval(primary_t *prm, scope_t *scope);
