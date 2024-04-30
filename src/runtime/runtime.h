#pragma once

#include <stdint.h>

#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/parser.h"

typedef enum {
  RUNTIME_CHAR,
  RUNTIME_STRING,
  RUNTIME_INT,
  RUNTIME_BOOL,
  RUNTIME_FN,
  RUNTIME_LIST,
} rt_type_t;

// Not sure if I'm satisfied with this (essentially duplicate from
// parser/constructor.h"
static const char *rt_type_map[] = {
    [RUNTIME_CHAR] = "char", [RUNTIME_STRING] = "string",
    [RUNTIME_INT] = "int",   [RUNTIME_BOOL] = "bool",
    [RUNTIME_FN] = "fn",     [RUNTIME_LIST] = "list",
};

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
