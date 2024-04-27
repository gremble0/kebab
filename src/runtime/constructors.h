#include "parser/constructors.h"
#include "runtime/runtime.h"

runtime_value_t *eval_primitive_constructor(constructor_t *constructor,
                                            scope_t *scope);
runtime_value_t *eval_fn_constructor(constructor_t *constructor,
                                     scope_t *scope);
runtime_value_t *eval_list_constructor(constructor_t *constructor,
                                       scope_t *scope);
