#include "runtime/statements.h"
#include "runtime/expressions.h"

void eval_definition(definition_t *def, scope_t *scope) {
  // Add symbol to scope
  // assign value to symbol

  switch (def->constructor->type) {
  case CONSTR_PRIMITIVE:
    break;
  case CONSTR_FN:
    break;
  case CONSTR_LIST:
    break;
  }
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
