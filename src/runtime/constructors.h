#include "parser/constructors.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_primitive_constructor(constructor_t *constr, scope_t *scope);
rt_value_t *eval_fn_constructor(constructor_t *constructor, scope_t *scope);
rt_value_t *eval_list_constructor(constructor_t *constructor, scope_t *scope);
