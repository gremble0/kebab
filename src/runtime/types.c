#include "runtime/types.h"
#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/constructors.h"
#include "runtime/error.h"

// TODO: This should not be necessary if no duplicate types are produced, then
// we could just do `==`
// have global type_registry and function type_register(keb_type_t *type) that
// checks if type_registry has type and adds if it doesnt -> no duplicate types
void type_compare(keb_type_t *expected, keb_type_t *actual) {
  if (expected->type != actual->type)
    err_type_error(type_to_string(expected)->s, type_to_string(actual)->s);

  switch (expected->type) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL:
    // Don't need deper comparisons for these types as any type errors will
    // be caught in the initial shallow check above
    break;

  case TYPE_LIST:
    type_compare(expected->list->type, actual->list->type);
    break;

  case TYPE_FN:
    type_compare(expected->fn->return_type, actual->fn->return_type);

    for (size_t i = 0; i < expected->fn->param_types->size; ++i)
      type_compare(list_get(expected->fn->param_types, i),
                   list_get(actual->fn->param_types, i));
    break;
  }
}

string_t *type_to_string(keb_type_t *type) {
  switch (type->type) {
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
    string_append(s, type_to_string(type->list->type));
    string_append_c(s, ')');
    return s;
  }
  case TYPE_FN: {
    // Should look something like "fn(int) => string"
    string_t *s = string_of_lit("fn(");
    list_t *params = type->fn->param_types;
    for (size_t i = 0; i < params->size; ++i) {
      fn_param_t *param = list_get(params, i);
      string_append(s, type_to_string(param->type));
    }
    string_append_lit(s, ") =>");
    string_append(s, type_to_string(type->fn->return_type));

    return s;
  }
  }
}
