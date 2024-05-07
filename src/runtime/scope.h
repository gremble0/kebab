#pragma once

#include "nonstdlib/nhashtable.h"
#include "runtime/runtime.h"

typedef struct scope_t scope_t;
struct scope_t {
  ht_t *bindings; // ht_t<char *, runtime_value_t>
  scope_t *outer;
};

scope_t *scope_init(scope_t *outer);
rt_value_t *scope_get(scope_t *scope, const char *name);
void scope_put(scope_t *scope, char *name, rt_value_t *v);
void scope_free(scope_t *scope);