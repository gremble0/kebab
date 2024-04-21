#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "parser/statements.h"

void eval_statement(statement_t *stmt) {}

void eval(ast_t *ast) { list_map(ast->stmts, (list_map_func)eval_statement); }
