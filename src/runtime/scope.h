#pragma once

#include "nonstdlib/nhashtable.h"

typedef struct scope_t scope_t;
struct scope_t {
  ht_t *bindings; // ht_t<char *, runtime_value_t>
  scope_t *outer;
};

scope_t *scope_init(scope_t *outer);
void scope_free(scope_t *scope);
