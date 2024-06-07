#pragma once

#include "parser/statements.h"
#include "runtime/scope.h"

void statement_eval(statement_t *stmt, scope_t *scope);
