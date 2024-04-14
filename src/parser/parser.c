#include <stdlib.h>

#include "lexer.h"
#include "nlist.h"
#include "parser.h"
#include "statements.h"
#include "utils.h"

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->stmts = list_init(LIST_START_SIZE, sizeof(statement_t));
  ast->strs = NULL; // only initialize when necessary (in build_strs function)

  while (lexer->cur_token->kind != TOKEN_EOF) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(ast->stmts, stmt);
    free(stmt);
  }

  FINISH_PARSING("ast");

  return ast;
}

/**
 * @brief Entrypoint for building a string representation of the AST
 */
void build_strs(ast_t *ast) {
  ast->strs = list_init(LIST_START_SIZE, sizeof(char *));
  list_push_back(ast->strs, "<ast>");

  for (size_t i = 0; i < ast->stmts->cur_size; ++i)
    statement_build_strs(list_get(ast->stmts, i), ast->strs);

  list_push_back(ast->strs, "</ast>");
}

// Cleanup functions

void ast_free(ast_t *ast) {
  list_free(ast->stmts, statement_free);
  if (ast->strs != NULL)
    list_free(ast->strs, free);
  free(ast);
}
