#include "runtime/statements.h"

void eval_definition(definition_t *def, scope_t *scope) {}
void eval_assignment(assignment_t *ass, scope_t *scope) {}
void eval_expression(expression_t *expr, scope_t *scope) {}

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
