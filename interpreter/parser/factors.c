#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/factors.h"
#include "parser/logging.h"

static unary_operator_t unary_operator_parse(lexer_t *lexer) {
  // TODO: some sort of error handling for invalid prefixes
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

factor_t *factor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("factor");

  factor_t *fac = malloc(sizeof(*fac));
  if (fac == NULL)
    err_malloc_fail();

  fac->span.file = lexer->file;
  fac->span.start = (position_t){lexer->line_number, lexer->line_pos};

  // Check for prefixes
  fac->prefix = unary_operator_parse(lexer);

  // Parse primary
  fac->primary = primary_parse(lexer);

  fac->span.end = (position_t){lexer->line_number, lexer->line_pos};

  PARSER_LOG_NODE_FINISH("factor");

  return fac;
}

void factor_free(factor_t *fac) {
  primary_free(fac->primary);
  free(fac);
}
