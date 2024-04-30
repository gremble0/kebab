#include <stdlib.h>

#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

void eval(ast_t *ast) {
  scope_t *scope = scope_init(NULL);

  for (size_t i = 0; i < ast->stmts->cur_size; ++i)
    eval_statement(list_get(ast->stmts, i), scope);
}
