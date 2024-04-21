#include <stdlib.h>

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "parser/statements.h"
#include "parser/utils.h"

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->stmts = list_init(LIST_START_SIZE); // list<statement_t *>

  while (lexer->cur_token->kind != TOKEN_EOF) {
    list_push_back(ast->stmts, parse_statement(lexer));
  }

  FINISH_PARSING("ast");

  return ast;
}

void ast_free(ast_t *ast) {
  list_map(ast->stmts, (list_map_func)statement_free);
  list_free(ast->stmts);

  free(ast);
}
