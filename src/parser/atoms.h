#pragma once

#include <stdint.h>

#include "lexer/lexer.h"

typedef enum {
  ATOM_CHAR,
  ATOM_STRING,
  ATOM_INT,
  ATOM_BOOL,
  ATOM_NAME,
} atom_type_t; // TODO: inline in struct below?

typedef struct {
  atom_type_t type;
  union {
    char *name;
    char *string_value;
    uint8_t char_value; // Maybe use more bits in the future for unicode
    int64_t int_value;
    uint8_t bool_value;

    // TODO:
    // float
    // nil
    // list?
    // map?
  };
} atom_t;

atom_t *parse_atom(lexer_t *lexer);
void atom_free(atom_t *atom);
