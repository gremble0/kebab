#include <stdio.h>
#include <stdlib.h>

#include "runtime/error.h"

_Noreturn void err_type_error(const char *expected, const char *actual) {
  fprintf(stderr, "type-error: expected type '%s', but got type '%s'\n",
          expected, actual);
  exit(1);
}
