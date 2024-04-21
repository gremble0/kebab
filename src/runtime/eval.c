#include "nonstdlib/nlist.h"
#include "parser/parser.h"

void eval_statement() {}

void eval(ast_t *ast) { list_map(ast->stmts, (list_map_func)eval_statement); }
