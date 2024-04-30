#include "parser/constructors.h"
#include "runtime/runtime.h"

rt_value_t *eval_char_constructor(constructor_t *constr, scope_t *scope);
rt_value_t *eval_string_constructor(constructor_t *constr, scope_t *scope);
rt_value_t *eval_int_constructor(constructor_t *constr, scope_t *scope);
rt_value_t *eval_bool_constructor(constructor_t *constr, scope_t *scope);
rt_value_t *eval_fn_constructor(constructor_t *constructor, scope_t *scope);
rt_value_t *eval_list_constructor(constructor_t *constructor, scope_t *scope);
