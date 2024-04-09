#include <stdlib.h>

#include "atoms.h"
#include "nerror.h"
#include "parser_error.h"
#include "string.h"
#include "utils.h"

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

  START_AND_FINISH_PARSING("atom");

  return atom;
}

void atom_free(atom_t *atom) {
  switch (atom->type) {
  case ATOM_STRING:
    free((void *)atom->string_value);
    break;
  case ATOM_NAME:
    free((void *)atom->name);
    break;
  // TODO: list atom
  default:
    break;
  }

  free(atom);
}
