#include <stdio.h>
#include <stdlib.h>

#include "parser/atoms.h"
#include "runtime/error.h"

// TODO: trace position need to take lexer as param? :(

_Noreturn void err_type_error(const char *expected, const char *actual) {
  fprintf(stderr, "type-error: expected type '%s', but got type '%s'\n",
          expected, actual);
  exit(1);
}

_Noreturn void err_list_type_error(const char *expected, const char *actual) {
  fprintf(stderr,
          "list-type-error: lists must be homogenous. Declared type '%s' "
          "actual type '%s'\n",
          expected, actual);
  exit(1);
}

// TODO: list all defined bindings for a scope?
_Noreturn void err_name_error(atom_t *name /* scope_t *scope */) {
  // TODO: print ^~~~~~ or something based on names span
  fprintf(stderr,
          "%s:%zu:%zu\n"
          "name-error: name '%s' is not defined in the current scope\n",
          name->span.file->name, name->span.start.line, name->span.start.col,
          name->name_value->s);
  exit(1);
}
