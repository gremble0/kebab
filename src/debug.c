#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "nerror.h"

char *make_indent(size_t indent_depth) {
  if (indent_depth == 0)
    return strdup("");

  char *result = malloc((sizeof(ONE_INDENT) - 1) * indent_depth + 1);
  if (!result)
    err_malloc_fail();

  char *ptr = result;
  for (size_t i = 0; i < indent_depth; ++i) {
    memcpy(ptr, ONE_INDENT, sizeof(ONE_INDENT) - 1);
    ptr += 2;
  }
  *ptr = '\0';

  return result;
}
