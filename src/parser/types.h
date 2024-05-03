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
  keb_type_kind_t type;
  union {
    keb_type_fn_t *fn;
    keb_type_list_t *list;
  };
};

static const char *type_kind_map[] = {
    [TYPE_CHAR] = "char", [TYPE_STRING] = "string", [TYPE_INT] = "int",
    [TYPE_BOOL] = "bool", [TYPE_FN] = "fn",         [TYPE_LIST] = "list",
};

keb_type_t *parse_type(lexer_t *lexer);
void type_free(keb_type_t *kt);
