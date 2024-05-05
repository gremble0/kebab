#pragma once

#include <stdint.h>

#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/parser.h"

typedef struct {
  keb_type_kind_t type;
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

rt_value_t *eval_add(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_minus(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_mult(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_div(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_lt(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_le(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_eq(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_neq(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_gt(rt_value_t *v1, rt_value_t *v2);
rt_value_t *eval_ge(rt_value_t *v1, rt_value_t *v2);
