#include <stdlib.h>

#include "parser/factors.h"
#include "parser/utils.h"

static unary_operator_t parse_unary_operator(lexer_t *lexer) {
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

#ifdef DEBUG_PARSER
  if (uo != UNARY_NO_OP)
    PARSER_LOG_NODE_SELF_CLOSING("factor_prefix");
#endif

  return uo;
}

factor_t *parse_factor(lexer_t *lexer) {
  PARSER_LOG_NODE_START("factor");

  factor_t *ft = malloc(sizeof(*ft));
  if (ft == NULL)
    err_malloc_fail();

  // Check for prefixes
  ft->prefix = parse_unary_operator(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

  PARSER_LOG_NODE_FINISH("factor");

  return ft;
}

void factor_free(factor_t *fac) {
  primary_free(fac->primary);
  free(fac);
}
