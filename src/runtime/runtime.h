#pragma once

#include <stdint.h>

#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "parser/types.h"

// typedef struct {
// } runtime_fn_t;
//
// typedef struct {
//   keb_type_t *type;
//   enum {
//     RUNTIME_CHAR,
//     RUNTIME_STRING,
//     RUNTIME_INT,
//     RUNTIME_FN,
//     RUNTIME_BOOL,
//   } type;
//   union {
//     char *string_value;
//     uint8_t char_value; // Maybe use more bits in the future for unicode
//     int64_t int_value;  // TODO: revisit ordering inside struct maybe?
//     uint8_t bool_value;
//   };
// } runtime_value_t;
//
// typedef struct {
//   char *symbol;
//   runtime_value_t *value;
// } binding_t;

typedef struct scope_t scope_t;
struct scope_t {
  list_t *bindings; // list<binding_t *>
  scope_t *outer;
};

void eval(ast_t *ast);
