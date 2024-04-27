#include <string.h>

#include "nonstdlib/nhashtable.h"
#include "runtime/constructors.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

// Add symbol to scope
// assign value to symbol
void eval_definition(definition_t *def, scope_t *scope) {
  runtime_value_t *rtv;

  switch (def->constructor->type) {
  case CONSTR_CHAR:
  case CONSTR_STRING:
  case CONSTR_INT:
  case CONSTR_BOOL:
    rtv = eval_primitive_constructor(def->constructor, scope);
    break;
  case CONSTR_FN:
    rtv = eval_fn_constructor(def->constructor, scope);
    break;
  case CONSTR_LIST:
    rtv = eval_list_constructor(def->constructor, scope);
    break;
  }

  ht_put(scope->bindings, def->name, strlen(def->name), rtv, sizeof(*rtv));
  // TODO: free(rtv) / fix nonstdlib ?
}

// Look up symbol in scope
// assign value to symbol if symbol is mutable
void eval_assignment(assignment_t *ass, scope_t *scope) {}

// Entrypoint for evaluating a statement
void eval_statement(statement_t *stmt, scope_t *scope) {
  switch (stmt->type) {
  case STMT_DEFINITION:
    eval_definition(stmt->definition, scope);
    break;
  case STMT_ASSIGNMENT:
    eval_assignment(stmt->assignment, scope);
    break;
  case STMT_EXPRESSION:
    eval_expression(stmt->expr, scope);
    break;
  }
}
