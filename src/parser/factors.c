#include <stdlib.h>

#include "parser/factors.h"
#include "parser/logging.h"

static unary_operator_t parse_unary_operator(lexer_t *lexer) {
  // TODO: some sort of error handling
  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    PARSER_LOG_NODE_SELF_CLOSING("factor-prefix='+'");
    lexer_advance(lexer);
    return UNARY_PLUS;
  case TOKEN_MINUS:
    PARSER_LOG_NODE_SELF_CLOSING("factor-prefix='-'");
    lexer_advance(lexer);
    return UNARY_MINUS;
  case TOKEN_NOT:
    PARSER_LOG_NODE_SELF_CLOSING("unary-operator='~'");
    lexer_advance(lexer);
    return UNARY_NOT;
  default:
    return UNARY_NO_OP;
  }
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
