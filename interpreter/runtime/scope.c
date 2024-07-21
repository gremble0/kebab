#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nhashtable.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "utils/utils.h"

scope_t *scope_init(const scope_t *outer) {
  scope_t *scope = malloc(sizeof(*scope));
  scope->outer = outer;
  scope->bindings = ht_init(TABLE_START_SIZE);

  return scope;
}

/**
 * @brief Look up a name in a given scope. Looks in outer scopes and returns the
 * value from the first scope that contains the name. Returns NULL if no outer
 * scopes contain the name
 *
 * @param scope scope to look for value in
 * @param name the name of the variable to look up
 * @return a pointer to some rt_value_t or NULL
 */
rt_value_t *scope_get(const scope_t *scope, const string_t *name) {
  while (scope != NULL) {
    rt_value_t *v = ht_get(scope->bindings, name);
    if (v != NULL)
      return v;

    scope = scope->outer;
  }

  return NULL;
}

void scope_put(scope_t *scope, string_t *name, rt_value_t *v) { ht_put(scope->bindings, name, v); }

void scope_free(scope_t *scope) {
  ht_free(scope->bindings);
  free(scope);
}
