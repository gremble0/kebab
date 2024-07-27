#include "runtime/builtin.h"
#include "runtime/scope.h"

static void load_print(scope_t *scope) {
  // TODO: implement
}

static void load_open(scope_t *scope) {
  // TODO: implement
}

void load_globals(scope_t *global_scope) {
  load_print(global_scope);
  load_open(global_scope);
}
