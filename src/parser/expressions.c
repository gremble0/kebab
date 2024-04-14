#include <stdlib.h>

#include "expressions.h"
#include "factors.h"
#include "nlist.h"
#include "utils.h"

static binary_operator_t *parse_binary_operator(lexer_t *lexer) {
  binary_operator_t *bo = malloc(sizeof(*bo));

  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    *bo = BINARY_PLUS;
    break;
  case TOKEN_MINUS:
    *bo = BINARY_MINUS;
    break;
  case TOKEN_MULT:
    *bo = BINARY_MULT;
    break;
  case TOKEN_DIV:
    *bo = BINARY_DIV;
    break;

  case TOKEN_LT:
    *bo = BINARY_LT;
    break;
  case TOKEN_LE:
    *bo = BINARY_LE;
    break;
  case TOKEN_EQ:
    *bo = BINARY_EQ;
    break;
  case TOKEN_NEQ:
    *bo = BINARY_NEQ;
    break;
  case TOKEN_GT:
    *bo = BINARY_GT;
    break;
  case TOKEN_GE:
    *bo = BINARY_GE;
    break;

    // If next token is not a binary operator set to no-op and return without
    // advancing lexer
  default:
    *bo = BINARY_NO_OP;
    goto skip;
  }

  lexer_advance(lexer);

START_AND_FINISH_PARSING("binary_operator");

skip:
  return bo;
}

expression_t *parse_expression(lexer_t *lexer) {
  START_PARSING("expr");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE, sizeof(factor_t));
  // TODO: don't always init operators
  expr->operators = list_init(LIST_START_SIZE, sizeof(binary_operator_t));

  // Continue parsing until there are no more binary operators
  while (1) {
    factor_t *ft = parse_factor(lexer);
    list_push_back(expr->factors, ft);
    free(ft);

    binary_operator_t *bo = parse_binary_operator(lexer);
    if (*bo == BINARY_NO_OP) {
      free(bo);
      break;
    }

    list_push_back(expr->operators, bo);
    free(bo);
  }

  FINISH_PARSING("expr");

  return expr;
}

/**
 * @param expr expression to free, should be type `expression_t`
 */
void expression_free(void *expr) {
  expression_t *e = expr;
  list_free(e->factors, factor_free);
  list_free(e->operators, free);
  free(expr);
}
