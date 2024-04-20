#pragma once

#include "lexer/lexer.h"
#include "nlist.h"

typedef struct keb_type_struct keb_type_t;

typedef struct {
  list_t *param_types; // list<keb_type_t *>
  keb_type_t *return_type;
} keb_type_fn_t;

typedef struct {
  keb_type_t *type;
} keb_type_list_t;

// TODO: create these types sparingly, as they will most of the time be
// duplicated
struct keb_type_struct {
  enum {
    TYPE_PRIMITIVE, // TODO: rename and use for user defined types when/if
                    // implemented?
    TYPE_LIST,
    TYPE_FN,
  } type;
  union {
    keb_type_fn_t *fn;
    keb_type_list_t *list;
    const char *name;
  };
};

keb_type_t *parse_type(lexer_t *lexer);
void type_free(keb_type_t *kt);
