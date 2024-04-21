#include <stdio.h>
#include <stdlib.h>

#include "atoms.h"
#include "error.h"
#include "nonstdlib/nerror.h"
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

char *atom_to_string(const atom_t *atom) {
  switch (atom->type) {
  case ATOM_CHAR: {
    char *res = malloc(sizeof("<atom type='char'> </atom>"));
    sprintf(res, "<atom type='char'>%c</atom>", atom->char_value);
    return res;
  }
  case ATOM_STRING: {
    char *res = malloc(sizeof("<atom type='string'></atom>") +
                       strlen(atom->string_value));
    sprintf(res, "<atom type='string'>%s</atom>", atom->string_value);
    return res;
  }
  case ATOM_INT: {
    int len = snprintf(NULL, 0, "%ld", atom->int_value);
    char *res = malloc(sizeof("<atom type='int'></atom>") + len);
    sprintf(res, "<atom type='int'>%ld</atom>", atom->int_value);
    return res;
  }
  case ATOM_BOOL: {
    if (atom->bool_value)
      return strdup("<atom type='bool'>true</atom>");
    else
      return strdup("<atom type='bool'>false</atom>");
  }
  case ATOM_NAME: {
    char *res =
        malloc(sizeof("<atom type='name'></atom>") + strlen(atom->name));
    sprintf(res, "<atom type='name'>%s</atom>", atom->name);
    return res;
  }
  }
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
