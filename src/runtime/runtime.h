#pragma once

#include <stdint.h>

#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/parser.h"

typedef enum {
  RUNTIME_CHAR,
  RUNTIME_STRING,
  RUNTIME_INT,
  RUNTIME_FN,
  RUNTIME_BOOL,
  RUNTIME_LIST,
} rt_type_t;

typedef struct {
  rt_type_t type;
  union {
    uint8_t char_value; // Maybe use more bits in the future for unicode
    char *string_value;
    int64_t int_value;
    uint8_t bool_value;
    fn_constructor_t *fn_value;
    list_t *list_value; // list<runtime_value_t*>
  };
} rt_value_t;

void eval(ast_t *ast);
