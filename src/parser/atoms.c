#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/atoms.h"
#include "parser/error.h"
#include "parser/expressions.h"
#include "parser/utils.h"
#include "string.h"

atom_t *parse_atom(lexer_t *lexer) {
  atom_t *atom = malloc(sizeof(*atom));
  if (atom == NULL)
    err_malloc_fail();

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR_LITERAL:
    atom->type = ATOM_CHAR;
    atom->char_value = lexer->cur_token->char_literal;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("char-literal='%c'", atom->char_value);
    break;

  case TOKEN_STRING_LITERAL:
    atom->type = ATOM_STRING;
    atom->string_value = strdup(lexer->cur_token->string_literal);
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("string-literal=\"%s\"", atom->string_value);
    break;

  case TOKEN_INTEGER_LITERAL:
    atom->type = ATOM_INT;
    atom->int_value = lexer->cur_token->integer_literal;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("integer-literal=%ld", atom->int_value);
    break;

  case TOKEN_NAME:
    atom->type = ATOM_NAME;
    atom->name = strdup(lexer->cur_token->name);
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("name=\"%s\"", atom->name);
    break;

  case TOKEN_TRUE:
    atom->type = ATOM_BOOL;
    atom->bool_value = 1;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("true");
    break;

  case TOKEN_FALSE:
    atom->type = ATOM_BOOL;
    atom->bool_value = 0;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("false");
    break;

  case TOKEN_LPAREN:
    atom->type = ATOM_INNER_EXPR;
    atom->inner_expr_value = parse_inner_expression(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  return atom;
}

void atom_free(atom_t *atom) {
  switch (atom->type) {
  case ATOM_STRING:
    free(atom->string_value);
    break;
  case ATOM_NAME:
    free(atom->name);
    break;
  case ATOM_INNER_EXPR:
    expression_free(atom->inner_expr_value);
    break;
  default:
    break;
  }

  free(atom);
}
