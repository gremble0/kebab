#include <stdlib.h>

#include "lexer.h"
#include "nlist.h"
#include "parser.h"
#include "statements.h"
#include "utils.h"

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->stmts = list_init(LIST_START_SIZE); // list<statement_t *>
  ast->strs = NULL; // only initialize when necessary (in build_strs function)

  while (lexer->cur_token->kind != TOKEN_EOF) {
    list_push_back(ast->stmts, parse_statement(lexer));
  }

  FINISH_PARSING("ast");

  return ast;
}

/**
 * @brief Entrypoint for building a string representation of the AST
 */
void build_strs(ast_t *ast) {
  ast->strs = list_init(LIST_START_SIZE); // list<char *>
  list_push_back(ast->strs, "<ast>");

  for (size_t i = 0; i < ast->stmts->cur_size; ++i)
    statement_build_strs(list_get(ast->stmts, i), ast->strs);

  list_push_back(ast->strs, "</ast>");
}

// Cleanup functions

void ast_free(ast_t *ast) {
  list_map(ast->stmts, (list_map_func)statement_free);
  list_free(ast->stmts);

  if (ast->strs != NULL) {
    list_map(ast->strs, free);
    list_free(ast->strs);
  }

  free(ast);
}
