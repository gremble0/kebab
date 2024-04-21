#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nerror.h"
#include "parser/utils.h"

static char *make_indent() {
  if (indent_depth == 0) {
    return strdup("");
  }

  char *indent = malloc((sizeof(ONE_INDENT) - 1) * indent_depth + 1);
  if (indent == NULL) {
    err_malloc_fail();
  }

  char *ptr = indent;
  for (size_t i = 0; i < indent_depth; ++i) {
    memcpy(ptr, ONE_INDENT, sizeof(ONE_INDENT) - 1);
    ptr += sizeof(ONE_INDENT) - 1;
  }
  *ptr = '\0';

  return indent;
}

void start_parsing(const char *node_name) {
  char *indent = make_indent();
  printf("%s<%s>\n", indent, node_name);
  free(indent);
  ++indent_depth;
}

void finish_parsing(const char *node_name) {
  --indent_depth;
  char *indent = make_indent();
  printf("%s</%s>\n", indent, node_name);
  free(indent);
}

/**
 * @brief Used to debug self closing nodes, e.g. nodes with no children.
 */
void start_and_finish_parsing(const char *node_name) {
  char *indent = make_indent();
  printf("%s<%s />\n", indent, node_name);
  free(indent);
}

/**
 * @brief Creates a new string consisting of the character `c` repeated `n`
 * times.
 *
 * @param c The character to repeat.
 * @param n The number of times to repeat `c`.
 * @return A malloc'd string of `c` repeated `n` times
 */
char *repeat_char(char c, size_t n) {
  char *str = malloc(n + 1);
  if (str == NULL) {
    err_malloc_fail();
  }

  memset(str, c, n);
  str[n] = '\0';

  return str;
}
