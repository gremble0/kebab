#pragma once

#include "lexer/lexer.h"
#include "nonstdlib/nlist.h"

typedef struct keb_type_t keb_type_t;

typedef struct {
  list_t *param_types; // list<keb_type_t *>
  keb_type_t *return_type;
} keb_type_fn_t;

typedef struct {
  keb_type_t *type;
} keb_type_list_t;

// TODO: create these types sparingly, as they will most of the time be
// duplicated. Something like a central register of created types and check if a
// type exists before making one
typedef enum {
  TYPE_CHAR,
  TYPE_STRING,
  TYPE_INT,
  TYPE_BOOL,
  TYPE_LIST,
  TYPE_FN,
} keb_type_kind_t;

struct keb_type_t {
  keb_type_kind_t kind;
  union {
    keb_type_fn_t fn_type;
    keb_type_list_t list_type;
  };
};

// TODO: Should be const but gives warnings
// extern to have the same address in all contexts (different files)
extern keb_type_t *type_char;
extern keb_type_t *type_string;
extern keb_type_t *type_int;
extern keb_type_t *type_bool;
extern keb_type_t *type_unparametrized_list;

// Make function? type_kind_to_string
static const char *type_kind_map[] = {
    [TYPE_CHAR] = "char", [TYPE_STRING] = "string", [TYPE_INT] = "int",
    [TYPE_BOOL] = "bool", [TYPE_FN] = "fn",         [TYPE_LIST] = "list",
};

keb_type_t *type_parse(lexer_t *lexer);
void type_free(keb_type_t *kt);
