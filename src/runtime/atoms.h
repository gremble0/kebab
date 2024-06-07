#pragma once

#include "parser/atoms.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *atom_eval(atom_t *atom, scope_t *scope);
