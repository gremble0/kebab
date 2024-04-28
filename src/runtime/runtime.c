#include <stdlib.h>

#include "nonstdlib/nhashtable.h"
#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"
#include "runtime/utils.h"

scope_t *scope_init(scope_t *outer) {
  scope_t *scope = malloc(sizeof(*scope));
  scope->outer = outer;
  scope->bindings = ht_init(LIST_START_SIZE); // TODO: maybe change macro here

  return scope;
}

void scope_free(scope_t *scope) {
  // TODO: map other function than free
  ht_free(scope->bindings);
  free(scope);
}

void eval(ast_t *ast) {
  scope_t *scope = scope_init(NULL);

  for (size_t i = 0; i < ast->stmts->cur_size; ++i)
    eval_statement(list_get(ast->stmts, i), scope);
}