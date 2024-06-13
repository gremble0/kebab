#include <stdio.h>
#include <stdlib.h>

#include "parser/types.h"
#include "runtime/error.h"
#include "runtime/types.h"
#include "utils/utils.h"

static void err_print_span(span_t span) {
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
}

/**
 * @brief A type error for parametrized types such as `list` and `fn` that are different at the top
 * level
 */
_Noreturn void err_opaque_type_error(keb_type_kind_t expected, keb_type_kind_t actual,
                                     span_t span) {
  err_print_span(span);

  fprintf(stderr, "opaque-type-error: expected type should be some '%s', but is some '%s'\n",
          type_kind_map[expected], type_kind_map[actual]);

  exit(1);
}

/**
 * @brief A type error where you expected one type but got a different one
 */
_Noreturn void err_type_error(keb_type_t *expected, keb_type_t *actual, span_t span) {
  err_print_span(span);

  const string_t *expected_str = type_to_string(expected);
  const string_t *actual_str = type_to_string(actual);
  fprintf(stderr, "type-error: expected type '%.*s', but got type '%.*s'\n", (int)expected_str->len,
          expected_str->s, (int)actual_str->len, actual_str->s);

  exit(1);
}

/**
 * @brief An error where the name has no binding in the given scope
 */
_Noreturn void err_name_error(string_t *name, span_t span /*, scope_t *scope */) {
  // TODO: list all defined bindings for a scope? call rt_value_to_string for each value in scope
  err_print_span(span);

  fprintf(stderr, "name-error: name '%s' is not defined in the current scope\n", name->s);

  exit(1);
}
