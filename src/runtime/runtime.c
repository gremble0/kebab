#include "parser/parser.h"
#include "runtime/statements.h"

void eval(ast_t *ast) { list_map(ast->stmts, (list_map_func)eval_statement); }
