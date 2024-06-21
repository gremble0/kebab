#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/constructors.h"
#include "parser/types.h"
#include "runtime/builtin.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

static void load_print(scope_t *scope) {
  rt_fn_t *keb_print_fn = malloc(sizeof(*keb_print_fn));
  if (keb_print_fn == NULL)
    err_malloc_fail();

  keb_print_fn->constr = malloc(sizeof(*keb_print_fn->constr));
  if (keb_print_fn->constr == NULL)
    err_malloc_fail();

  // Describe the type of the print function
  keb_type_t *keb_print_type = malloc(sizeof(*keb_print_type));
  if (keb_print_type == NULL)
    err_malloc_fail();
  keb_print_type->kind = TYPE_FN;
  keb_print_type->fn_type.return_type = NULL;
  // Only one param so we don't need more capacity
  keb_print_type->fn_type.param_types = list_init(1);
  list_push_back(keb_print_type->fn_type.param_types, type_string);
  keb_print_fn->constr->type = keb_print_type;

  // Describe the parameters to the print function
  fn_param_t *keb_print_param = malloc(sizeof(*keb_print_param));
  if (keb_print_param == NULL)
    err_malloc_fail();
  keb_print_param->type = type_string;
  keb_print_param->name = string_dup(&string_of("s"));
  keb_print_fn->params = list_init(1);
  list_push_back(keb_print_fn->params, keb_print_param);

  // Describe the body of the print function
  constructor_fn_t *keb_print_fn_constr = malloc(sizeof(*keb_print_fn_constr));
  if (keb_print_param == NULL)
    err_malloc_fail();
  // keb_print_fn->constr->fn_constructor = mal

  // Make a runtime value from the print function
  rt_value_t *keb_print = malloc(sizeof(*keb_print));
  if (keb_print == NULL)
    err_malloc_fail();
  keb_print->type = keb_print_type;
  keb_print->fn_value = keb_print_fn;

  // Insert the print function into the provided scope
  scope_put(scope, string_dup(&string_of("print")), keb_print);
}

static void load_open(scope_t *scope) {}

void load_globals(scope_t *global_scope) {
  load_print(global_scope);
  load_open(global_scope);
}
