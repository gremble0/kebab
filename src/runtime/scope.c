#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nhashtable.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/utils.h"

scope_t *scope_init(scope_t *outer) {
  scope_t *scope = malloc(sizeof(*scope));
  scope->outer = outer;
  scope->bindings = ht_init(TABLE_START_SIZE);

  return scope;
}

// TODO: use string_t instead of char* to avoid strlen

rt_value_t *scope_get(scope_t *scope, const char *name) {
  return ht_get(scope->bindings, name, strlen(name));
}

void scope_put(scope_t *scope, char *name, rt_value_t *v) {
  ht_put(scope->bindings, name, strlen(name), v);
}

void scope_free(scope_t *scope) {
  ht_free(scope->bindings);
  free(scope);
}
