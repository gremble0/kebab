#include "runtime/statements.h"

void eval_definition(definition_t *def) {}

void eval_statement(statement_t *stmt) {
  switch (stmt->type) {
  case STMT_DEFINITION:
  case STMT_ASSIGNMENT:
  case STMT_EXPRESSION:
    break;
  }
}
