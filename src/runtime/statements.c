#include "runtime/statements.h"
#include "nonstdlib/nerror.h"
#include "runtime/constructors.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

// Add symbol to scope
// assign value to symbol
static void definition_eval(definition_t *def, scope_t *scope) {
  rt_value_t *v = constructor_eval(def->constructor, scope);
  scope_put(scope, def->name, v);
}

// Look up symbol in scope
// assign value to symbol if symbol is mutable
static void assignment_eval(assignment_t *ass, scope_t *scope) {
  rt_value_t *existing = scope_get(scope, ass->name);

  // Assigning to non existent symbol is an error
  if (existing == NULL)
    ASSERT(0);

  // TODO: rt_value_t should have a mutable flag
}

// Entrypoint for evaluating a statement
void statement_eval(statement_t *stmt, scope_t *scope) {
  switch (stmt->type) {
  case STMT_DEFINITION:
    definition_eval(stmt->definition, scope);
    break;
  case STMT_ASSIGNMENT:
    assignment_eval(stmt->assignment, scope);
    break;
  case STMT_EXPRESSION:
    expression_eval(stmt->expr, scope);
    break;
  }
}
