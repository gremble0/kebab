#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "statements.h"
#include "utils.h"

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->stmts = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_EOF) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(ast->stmts, stmt);
    free(stmt);
  }

  FINISH_PARSING("ast");

  return ast;
}

// Cleanup functions

void ast_free(ast_t *ast) {
  list_free(ast->stmts, statement_free);
  free(ast);
}
