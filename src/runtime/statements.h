#pragma once

#include "parser/statements.h"
#include "runtime/scope.h"

void eval_statement(statement_t *stmt, scope_t *scope);
