#include <stdlib.h>

#include "nonstdlib/nlist.h"
#include "runtime/constructors.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

// Add symbol to scope
// assign value to symbol
void eval_definition(definition_t *def, scope_t *scope) {
  binding_t *bnd = malloc(sizeof(*bnd));
  bnd->symbol = def->name;

  switch (def->constructor->type) {
  case CONSTR_PRIMITIVE:
    bnd->value = eval_primitive_constructor(def->constructor, scope);
    break;
  case CONSTR_FN:
    bnd->value = eval_fn_constructor(def->constructor, scope);
    break;
  case CONSTR_LIST:
    bbd->value = eval_list_constructor(def->constructor, scope);
    break;
  }

  list_push_back(scope->bindings, bnd);
}
void eval_assignment(assignment_t *ass, scope_t *scope) {
  // Look up symbol in scope
  // assign value to symbol if symbol is mutable
}

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
