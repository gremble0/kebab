#include <stdio.h>
#include <stdlib.h>

#include "parser/atoms.h"
#include "runtime/error.h"

_Noreturn void err_type_error(const char *expected, const char *actual) {
  // Error description
  fprintf(stderr, "type-error: expected type '%s', but got type '%s'\n",
          expected, actual);
  exit(1);
}

// TODO: list all defined bindings for a scope?
_Noreturn void err_name_error(atom_t *name /*, scope_t *scope */) {
  // Error description
  fprintf(stderr, "name-error: name '%s' is not defined in the current scope\n",
          name->name_value->s);
  exit(1);
}
