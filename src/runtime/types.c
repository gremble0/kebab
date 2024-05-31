#include "runtime/types.h"
#include "nonstdlib/nlist.h"
#include "runtime/error.h"

void type_compare(keb_type_t *kt1, keb_type_t *kt2) {
  switch (kt1->type) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL:
    if (kt1->type != kt2->type)
      err_type_error("idk", "idk2");
    break;

  case TYPE_LIST:
    type_compare(kt1->list->type, kt2->list->type);
    break;

  case TYPE_FN:
    type_compare(kt1->fn->return_type, kt2->fn->return_type);

    for (size_t i = 0; i < kt1->fn->param_types->size; ++i)
      type_compare(list_get(kt1->fn->param_types, i),
                   list_get(kt2->fn->param_types, i));
    break;
  }
}
