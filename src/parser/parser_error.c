#include <stdio.h>
#include <stdlib.h>

#include "nerror.h"
#include "parser_error.h"

_Noreturn void err_wrong_token(const char *expected, token_t *actual) {
  err_print_stacktrace();
  fprintf(stderr, "ERR_WRONG_TOKEN: expected '%s', got '%s'\n", expected,
          token_to_string(actual));
  exit(1);
}

_Noreturn void err_illegal_token(token_t *token) {
  err_print_stacktrace();
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: %s'\n", token_to_string(token));
  exit(1);
}

_Noreturn void err_set_constant(token_t *constant) {
  err_print_stacktrace();
  fprintf(stderr,
          "ERR_SET_CONSTANT: cannot mutate '%s' with 'set' assignment. Define "
          "variable with 'mut'\n",
          token_to_string(constant));
  exit(1);
}
