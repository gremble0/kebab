#pragma once

#include <stdint.h>

#include "lexer/lexer.h"

typedef enum {
  ATOM_CHAR,
  ATOM_STRING,
  ATOM_INT,
  ATOM_BOOL,
  ATOM_NAME,
} atom_type_t;

typedef struct {
  atom_type_t type;
  union {
    char char_value;
    char *string_value;
    int64_t int_value;
    char bool_value;
    char *name;

    // TODO:
    // float
    // nil
    // list?
    // map?
  };
} atom_t;

atom_t *parse_atom(lexer_t *lexer);
char *atom_to_string(const atom_t *atom);
void atom_free(atom_t *atom);
