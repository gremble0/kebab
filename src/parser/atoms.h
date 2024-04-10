#pragma once

#include "lexer.h"
#include "nlist.h"

typedef enum {
  ATOM_CHAR,
  ATOM_STRING,
  ATOM_INT,
  ATOM_BOOL,
  ATOM_NAME,
  ATOM_LIST,
} atom_type_t;

typedef struct {
  atom_type_t type;
  union {
    char char_value;
    const char *string_value;
    int int_value;
    char bool_value;
    const char *name;
    list_t *list_value;

    // TODO:
    // float
    // nil
    // bool
    // map?
  };
} atom_t;

atom_t *parse_atom(lexer_t *lexer);
char *atom_type_to_string(atom_type_t at);
void atom_free(atom_t *atom);
