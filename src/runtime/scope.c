#include <stdlib.h>

#include "runtime/scope.h"
#include "runtime/utils.h"

// TODO: scope to own file?
scope_t *scope_init(scope_t *outer) {
  scope_t *scope = malloc(sizeof(*scope));
  scope->outer = outer;
  scope->bindings = ht_init(TABLE_START_SIZE);

  return scope;
}

void scope_free(scope_t *scope) {
  ht_free(scope->bindings);
  free(scope);
}
