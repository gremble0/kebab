#include <stdio.h>
#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/atoms.h"
#include "runtime/error.h"
#include "utils/utils.h"

_Noreturn void err_type_error(const char *expected, const char *actual) {
  // Error description
  fprintf(stderr, "type-error: expected type '%s', but got type '%s'\n", expected, actual);
  exit(1);
}

// TODO: list all defined bindings for a scope?
_Noreturn void err_name_error(atom_t *name /*, scope_t *scope */) {
  ASSERT(name->type == ATOM_NAME);
  span_t span = name->span;

  // Coordinates for where the error comes from
  fprintf(stderr, "%s:%zu:%zu\n", span.file.name, span.start.line, span.start.col);

  // The line the error comes from
  const string_t *line = get_line_from_file(span.file.f, span.start.line);
  fprintf(stderr, "%.*s", (int)line->len, line->s);

  // The position on the line the error comes from
  const string_t *indent = repeat_char(' ', span.start.col);
  // - 1 because span is inclusive, we want exclusive here to not underline trailing spaces, parens
  const string_t *underline = repeat_char('~', span.end.col - span.start.col - 1);
  fprintf(stderr, "%.*s^%.*s\n", (int)indent->len, indent->s, (int)underline->len, underline->s);

  // Error description
  fprintf(stderr, "name-error: name '%s' is not defined in the current scope\n",
          name->name_value->s);

  exit(1);
}
