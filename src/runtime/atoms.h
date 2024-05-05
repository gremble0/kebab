#include "parser/atoms.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_atom(atom_t *atom, scope_t *scope);
