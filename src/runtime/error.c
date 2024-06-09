#include <stdio.h>
#include <stdlib.h>

#include "parser/atoms.h"
#include "parser/parser.h"
#include "runtime/error.h"
#include "utils/utils.h"

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
_Noreturn void err_name_error(atom_t *name /*, scope_t *scope */) {
  // TODO: print ^~~~~~ or something based on names span
  span_t span = name->span;

  fprintf(stderr,
          "%s:%zu:%zu\n"
          "%s\n" // Should be the line the error comes from
          // "\n"   // Should be ^~~~~~ under the erroring part of the line
          "name-error: name '%s' is not defined in the current scope\n",
          span.file->name, span.start.line, span.start.col,
          get_line_from_file(span.file->f, span.start.line),
          name->name_value->s);
  exit(1);
}
