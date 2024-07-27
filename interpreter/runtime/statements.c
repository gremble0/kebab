#include "runtime/statements.h"
#include "runtime/constructors.h"
#include "runtime/error.h"
#include "runtime/expressions.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/types.h"

// Add symbol to scope
// assign value to symbol
static void definition_eval(definition_t *def, scope_t *scope) {
  rt_value_t *v = constructor_eval(def->constructor, scope);
  v->is_mutable = def->is_mutable;
  scope_put(scope, def->name, v);
}

// Look up symbol in scope
// assign value to symbol if symbol is mutable
static void assignment_eval(assignment_t *ass, scope_t *scope, span_t span) {
  rt_value_t *existing = scope_get(scope, ass->name);

  // Assigning to non existent symbol is an error
  if (existing == NULL)
    err_name_error(ass->name, span, scope);
  else if (!existing->is_mutable)
    err_set_constant(ass->name, span);

  // TODO: will leak memory (previous value is lost)
  // rt_value_free(existing) ?
  rt_value_t *new = constructor_eval(ass->constructor, scope);

  type_compare(existing->type, new->type, span);
  *existing = *new;
}

// Entrypoint for evaluating a statement
void statement_eval(statement_t *stmt, scope_t *scope) {
  switch (stmt->type) {
  case STMT_DEFINITION:
    definition_eval(stmt->definition, scope);
    break;

  case STMT_ASSIGNMENT:
    assignment_eval(stmt->assignment, scope, stmt->span);
    break;

  case STMT_EXPRESSION:
    expression_eval(stmt->expr, scope);
    break;
  }
}
