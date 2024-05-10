#include <stdlib.h>

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"
#include "parser/expressions.h"
#include "parser/logging.h"
#include "parser/primaries.h"

static list_t *primary_arguments_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primary-arguments");
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  list_t *args = list_init(LIST_START_SIZE); // list<expression_t *>

  // Here we could handle a small portion of wrong function calls (e.g. `1()`,
  // `""()`, etc), but we will leave them all to the runtime instead
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(args, parse_expression(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN)
      SKIP_TOKEN(lexer, TOKEN_COMMA);
  }

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("primary-arguments");

  return args;
}

primary_t *primary_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primary");

  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL)
    err_malloc_fail();

  prm->atom = parse_atom(lexer);
  if (lexer->cur_token->kind == TOKEN_LPAREN)
    prm->arguments = primary_arguments_parse(lexer);
  else
    prm->arguments = NULL;

  PARSER_LOG_NODE_FINISH("primary");

  return prm;
}

void primary_free(primary_t *prm) {
  free_atom(prm->atom);

  if (prm->arguments != NULL) {
    list_map(prm->arguments, (list_map_func)free_expression);
    list_free(prm->arguments);
  }

  free(prm);
}
