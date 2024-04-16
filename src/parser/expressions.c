#include <stdlib.h>

#include "expressions.h"
#include "factors.h"
#include "nlist.h"
#include "utils.h"

static binary_operator_t *parse_binary_operator(lexer_t *lexer) {
  binary_operator_t bo;

  switch (lexer->cur_token->kind) {
  // Maths
  case TOKEN_PLUS:
    bo = BINARY_PLUS;
    break;
  case TOKEN_MINUS:
    bo = BINARY_MINUS;
    break;
  case TOKEN_MULT:
    bo = BINARY_MULT;
    break;
  case TOKEN_DIV:
    bo = BINARY_DIV;
    break;

  // Comparisons
  case TOKEN_LT:
    bo = BINARY_LT;
    break;
  case TOKEN_LE:
    bo = BINARY_LE;
    break;
  case TOKEN_EQ:
    bo = BINARY_EQ;
    break;
  case TOKEN_NEQ:
    bo = BINARY_NEQ;
    break;
  case TOKEN_GT:
    bo = BINARY_GT;
    break;
  case TOKEN_GE:
    bo = BINARY_GE;
    break;

  // If next token is not a binary operator return without further processing
  default:
    return NULL;
  }

  lexer_advance(lexer);

  START_AND_FINISH_PARSING("binary_operator");

  binary_operator_t *ret = malloc(sizeof(*ret));
  *ret = bo;

  return ret;
}

expression_t *parse_expression(lexer_t *lexer) {
  START_PARSING("expr");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE); // list<factor_t *>
  // TODO: don't always init operators
  expr->operators = list_init(LIST_START_SIZE); // list<binary_operator_t *>

  // Continue parsing until there are no more binary operators
  while (1) {
    list_push_back(expr->factors, parse_factor(lexer));

    binary_operator_t *bo = parse_binary_operator(lexer);
    if (bo == NULL)
      break;

    list_push_back(expr->operators, bo);
  }

  FINISH_PARSING("expr");

  return expr;
}

/**
 * @param expr expression to free, should be type `expression_t`
 */
void expression_free(expression_t *expr) {
  list_map(expr->factors, (list_map_func)factor_free);
  list_map(expr->operators, free);

  list_free(expr->factors);
  list_free(expr->operators);
  free(expr);
}
