#include <stdlib.h>

#include "lexer.h"
#include "nlist.h"
#include "parser.h"
#include "parser_error.h"
#include "statements.h"
#include "token.h"
#include "utils.h"

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_EOF) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(ast->statements, stmt);
    free(stmt);
  }

  FINISH_PARSING("ast");

  return ast;
}

// Cleanup functions

void ast_free(ast_t *ast) {
  list_free(ast->statements, statement_free);
  free(ast);
}
