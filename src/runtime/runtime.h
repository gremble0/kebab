#pragma once

#include <stdint.h>

#include "nonstdlib/nhashtable.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/parser.h"

typedef struct {
  enum {
    RUNTIME_CHAR,
    RUNTIME_STRING,
    RUNTIME_INT,
    RUNTIME_FN,
    RUNTIME_BOOL,
    RUNTIME_LIST,
  } type;
  union {
    uint8_t char_value; // Maybe use more bits in the future for unicode
    char *string_value;
    int64_t int_value;
    uint8_t bool_value;
    fn_constructor_t *fn_value;
    list_t *list_value; // list<runtime_value_t*>
  } value;
} runtime_value_t;

typedef struct {
  char *symbol;
  runtime_value_t *value;
} binding_t;

typedef struct scope_t scope_t;
struct scope_t {
  ht_t *bindings; // ht_t<char *, runtime_value_t>
  scope_t *outer;
};

scope_t *scope_init(scope_t *outer);
void scope_free(scope_t *scope);
void eval(ast_t *ast);
