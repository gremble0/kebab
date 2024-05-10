#include "runtime/statements.h"
#include "nonstdlib/nerror.h"
#include "runtime/constructors.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

// Add symbol to scope
// assign value to symbol
void eval_definition(definition_t *def, scope_t *scope) {
  rt_value_t *v = eval_constructor(def->constructor, scope);
  scope_put(scope, def->name->s, v);
}

// Look up symbol in scope
// assign value to symbol if symbol is mutable
void eval_assignment(assignment_t *ass, scope_t *scope) {
  rt_value_t *existing = scope_get(scope, ass->name->s);

  // Assigning to non existent symbol is an error
  if (existing == NULL)
    ASSERT(0);

  // TODO: rt_value_t should have a mutable flag
}

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
