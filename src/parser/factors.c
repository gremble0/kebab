#include <stdlib.h>

#include "factors.h"
#include "utils.h"

// TODO: rename -> parse_unary_operator?
static unary_operator_t parse_factor_prefix(lexer_t *lexer) {
  unary_operator_t uo;

  // TODO: some sort of error handling
  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    lexer_advance(lexer);
    uo = UNARY_PLUS;
    break;
  case TOKEN_MINUS:
    lexer_advance(lexer);
    uo = UNARY_MINUS;
    break;
  case TOKEN_NOT:
    lexer_advance(lexer);
    uo = UNARY_NOT;
    break;
  default:
    uo = UNARY_NO_OP;
  }

#ifdef DEBUG
  if (uo != UNARY_NO_OP)
    START_AND_FINISH_PARSING("factor_prefix");
#endif

  return uo;
}

factor_t *parse_factor(lexer_t *lexer) {
  START_PARSING("factor");

  factor_t *ft = malloc(sizeof(*ft));
  if (ft == NULL) {
    lexer_free(lexer); // TODO: add macro/function for this repetetive cleanup?
    err_malloc_fail(); // TODO: add malloc fail checks everywhere.
  }

  // Check for prefixes
  ft->prefix = parse_factor_prefix(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

  FINISH_PARSING("factor");

  return ft;
}

/**
 * @param fac factor to free, should be type `factor_t`
 */
void factor_free(factor_t *fac) {
  primary_free(fac->primary);
  free(fac);
}
