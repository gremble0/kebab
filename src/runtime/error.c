#include <stdio.h>
#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nstring.h"
#include "parser/types.h"
#include "runtime/error.h"
#include "runtime/types.h"
#include "utils/utils.h"

// NOTE: the functions marked with _Noreturn will not free up any memory in use before exiting,
// relying on the operating system to reclaim the memory for the user (which any self respecting OS
// will do, so this should not be an issue)

static void err_print_span_multiline(span_t span) {
  // Coordinates for where the error comes from (A range of two coordinates in this case)
  fprintf(stderr, "%s:%zu:%zu-%zu:%zu\n", span.file.name, span.start.line, span.start.col,
          span.end.line, span.end.col);

  // First line the error comes from
  string_t *first_line = get_line_from_file(span.file.f, span.start.line);
  fprintf(stderr, "%.*s", (int)first_line->len, first_line->s);

  // ...
  fprintf(stderr, "...\n");

  // Last line the error comes from
  string_t *last_line = get_line_from_file(span.file.f, span.end.line);
  fprintf(stderr, "%.*s", (int)last_line->len, last_line->s);

  string_free(first_line);
  string_free(last_line);
}

static void err_print_span_oneline(span_t span) {
  // Coordinates for where the error comes from
  fprintf(stderr, "%s:%zu:%zu\n", span.file.name, span.start.line, span.start.col);

  // The line the error comes from
  string_t *line = get_line_from_file(span.file.f, span.start.line);
  fprintf(stderr, "%.*s", (int)line->len, line->s);

  // The position on the line the error comes from
  string_t *indent = repeat_char(' ', span.start.col);
  ASSERT(span.end.col - span.start.col - 1 >= 0);
  string_t *underline = repeat_char('~', span.end.col - span.start.col - 1);
  fprintf(stderr, "%.*s^%.*s\n", (int)indent->len, indent->s, (int)underline->len, underline->s);

  // It's not really necessary to free memory here since we this function should only be called from
  // non returning functions, but since this function itself will return we free the memory just in
  // case
  string_free(line);
  string_free(indent);
  string_free(underline);
}

static void err_print_span(span_t span) {
  if (span.end.line - span.start.line == 0)
    err_print_span_oneline(span);
  else
    err_print_span_multiline(span);
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
