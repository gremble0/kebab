#include <stdlib.h>

#include "expressions.h"
#include "factors.h"
#include "utils.h"

static binary_operator_t parse_binary_operator(lexer_t *lexer) {
  binary_operator_t bo;

  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    lexer_advance(lexer);
    bo = BINARY_PLUS;
    break;
  case TOKEN_MINUS:
    lexer_advance(lexer);
    bo = BINARY_MINUS;
    break;
  case TOKEN_MULT:
    lexer_advance(lexer);
    bo = BINARY_MULT;
    break;
  case TOKEN_DIV:
    lexer_advance(lexer);
    bo = BINARY_DIV;
    break;
  default:
    bo = BINARY_NO_OP;
  }

#ifdef DEBUG
  if (bo != BINARY_NO_OP)
    start_and_finish_parsing("binary_operator");
#endif

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

  // First parse one factor and binary operator
  factor_t *factor = parse_factor(lexer);
  list_push_back(expr->factors, factor);
  free(factor);
  binary_operator_t bo = parse_binary_operator(lexer);

  // Continue parsing until there are no more binary operators
  while (bo != BINARY_NO_OP) {
    switch (lexer->cur_token->kind) {
    case TOKEN_CHAR_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_NAME: {
      binary_operator_t *bo_p = malloc(sizeof(*bo_p));
      *bo_p = bo;
      factor_t *factor = parse_factor(lexer);

      // Add the operator from the last iteration, and the factor for this
      // iteration
      list_push_back(expr->operators, bo_p);
      list_push_back(expr->factors, factor);

      free(bo_p);
      free(factor);

      bo = parse_binary_operator(lexer); // operator for next iteration

      break;
    }

    default:
      err_illegal_token(lexer);
    }
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
