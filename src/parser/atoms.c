#include <stdio.h>
#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/atoms.h"
#include "parser/error.h"
#include "parser/utils.h"
#include "string.h"

atom_t *parse_atom(lexer_t *lexer) {
  // TODO: maybe free some strings here?
  atom_t *atom = malloc(sizeof(*atom));
  if (atom == NULL) {
    err_malloc_fail();
  }

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR_LITERAL:
    atom->type = ATOM_CHAR;
    atom->char_value = lexer->cur_token->char_literal;
    break;
  case TOKEN_STRING_LITERAL:
    atom->type = ATOM_STRING;
    atom->string_value = strdup(lexer->cur_token->string_literal);
    break;
  case TOKEN_INTEGER_LITERAL:
    atom->type = ATOM_INT;
    atom->int_value = lexer->cur_token->integer_literal;
    break;
  case TOKEN_NAME:
    atom->type = ATOM_NAME;
    atom->name = strdup(lexer->cur_token->name);
    break;
  // TODO: list, nil, bool, etc.
  default:
    err_illegal_token(lexer);
  }

  lexer_advance(lexer);

  PARSER_LOG_NODE_SELF_CLOSING("atom");

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
  // TODO: list atom
  default:
    break;
  }

  free(atom);
}
