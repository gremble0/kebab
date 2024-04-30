#include <stdlib.h>

#include "nonstdlib/nhashtable.h"
#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"
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

void eval(ast_t *ast) {
  scope_t *scope = scope_init(NULL);

  for (size_t i = 0; i < ast->stmts->cur_size; ++i)
    eval_statement(list_get(ast->stmts, i), scope);
}
