#include "runtime/types.h"
#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/types.h"
#include "runtime/error.h"

// TODO: This should not be necessary if no duplicate types are produced, then
// we could just do `==`
// have global type_registry and function type_register(keb_type_t *type) that
// checks if type_registry has type and adds if it doesnt -> no duplicate types
void type_compare(keb_type_t *expected, keb_type_t *actual, span_t span) {
  if (expected->kind != actual->kind)
    err_type_error(expected, actual, span);

  switch (expected->kind) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL:
    // Don't need deper comparisons for these types as any type errors will
    // be caught in the initial shallow check above
    break;

  case TYPE_LIST:
    type_compare(expected->list_type.type, actual->list_type.type, span);
    break;

  case TYPE_FN:
    type_compare(expected->fn_type.return_type, actual->fn_type.return_type, span);

    for (size_t i = 0; i < expected->fn_type.param_types->size; ++i)
      type_compare(list_get(expected->fn_type.param_types, i),
                   list_get(actual->fn_type.param_types, i), span);
    break;
  }
}

string_t *type_to_string(keb_type_t *type) {
  switch (type->kind) {
  case TYPE_CHAR:
    return string_of_lit("char");

  case TYPE_STRING:
    return string_of_lit("string");

  case TYPE_INT:
    return string_of_lit("int");

  case TYPE_BOOL:
    return string_of_lit("bool");

  case TYPE_LIST: {
    // Should look something like "list(string)"
    string_t *s = string_of_lit("list(");
    string_append(s, type_to_string(type->list_type.type));
    string_append_c(s, ')');
    return s;
  }

  case TYPE_FN: {
    // Should look something like "fn(int) => string"
    string_t *s = string_of_lit("fn(");
    list_t *param_types = type->fn_type.param_types;

    for (size_t i = 0; i < param_types->size; ++i) {
      string_append(s, type_to_string(list_get(param_types, i)));
      if (i < param_types->size - 1)
        string_append_lit(s, ", ");
    }

    string_append_lit(s, ") => ");
    string_append(s, type_to_string(type->fn_type.return_type));

    return s;
  }
  }
}
