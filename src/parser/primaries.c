#include <stdlib.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/expressions.h"
#include "parser/logging.h"
#include "parser/primaries.h"
#include "utils/utils.h"

static list_t *primary_arguments_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primary-arguments");
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  list_t *args = list_init(LIST_START_SIZE); // list<expression_t *>

  // Here we could handle a small portion of wrong function calls (e.g. `1()`,
  // `""()`, etc), but we will leave them all to the runtime instead
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(args, expression_parse(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN)
      SKIP_TOKEN(lexer, TOKEN_COMMA);
  }

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("primary-arguments");

  return args;
}

static expression_t *primary_subscription_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primary-subscription");
  SKIP_TOKEN(lexer, TOKEN_LBRACKET);

  expression_t *subscription = expression_parse(lexer);

  SKIP_TOKEN(lexer, TOKEN_RBRACKET);
  PARSER_LOG_NODE_FINISH("primary-subscription");

  return subscription;
}

static primary_suffix_t *primary_suffix_parse(lexer_t *lexer) {
  if (lexer->cur_token->kind == TOKEN_LPAREN) {
    // (...) -> arguments for function call
    primary_suffix_t *psfx = malloc(sizeof(*psfx));
    psfx->type = PRIMARY_ARGUMENT;
    psfx->arguments = primary_arguments_parse(lexer);

    return psfx;
  } else if (lexer->cur_token->kind == TOKEN_LBRACKET) {
    // [...] -> bracketed expression for subscripting a list
    primary_suffix_t *psfx = malloc(sizeof(*psfx));
    psfx->type = PRIMARY_SUBSCRIPTION;
    psfx->subscription = primary_subscription_parse(lexer);

    return psfx;
  } else {
    return NULL;
  }
}

primary_t *primary_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primary");

  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL)
    err_malloc_fail();

  prm->atom = atom_parse(lexer);
  if (lexer->cur_token->kind == TOKEN_LPAREN || lexer->cur_token->kind == TOKEN_LBRACKET) {
    prm->suffixes = list_init(LIST_START_SIZE);

    // There could be multiple sequenced suffixes, e.g. my-function()[1][2]
    while (lexer->cur_token->kind == TOKEN_LPAREN || lexer->cur_token->kind == TOKEN_LBRACKET)
      list_push_back(prm->suffixes, primary_suffix_parse(lexer));
  } else {
    prm->suffixes = NULL;
  }

  PARSER_LOG_NODE_FINISH("primary");

  return prm;
}

static void primary_suffix_free(primary_suffix_t *psfx) {
  switch (psfx->type) {
  case PRIMARY_SUBSCRIPTION:
    expression_free(psfx->subscription);
    // free(psfx->subscription) ?
    break;
  case PRIMARY_ARGUMENT:
    list_map(psfx->arguments, (list_map_func)expression_free);
    list_free(psfx->arguments);
    break;
  }

  free(psfx);
}

void primary_free(primary_t *prm) {
  atom_free(prm->atom);

  if (prm->suffixes != NULL) {
    list_map(prm->suffixes, (list_map_func)primary_suffix_free);
    list_free(prm->suffixes);
  }

  free(prm);
}
