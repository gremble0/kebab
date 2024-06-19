#pragma once

#include <stdint.h>

#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/parser.h"
#include "parser/types.h"

typedef constructor_fn_t rt_func_t;
typedef list_t rt_list_t;

typedef struct {
  keb_type_t *type;
  union {
    uint8_t char_value; // Maybe use more bits in the future for unicode
    string_t *string_value;
    int64_t int_value;
    uint8_t bool_value;
    rt_func_t *fn_value;
    rt_list_t *list_value; // list<rt_value_t*>
  };
} rt_value_t;

void eval(ast_t *ast);
string_t *rt_value_to_string(const rt_value_t *v);
