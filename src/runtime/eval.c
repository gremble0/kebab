#include "nlist.h"
#include "parser.h"
#include "statements.h"

void eval_statement(statement_t *stmt) {}

void eval(ast_t *ast) { list_map(ast->stmts, (list_map_func)eval_statement); }
