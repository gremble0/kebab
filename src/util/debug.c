#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "nerror.h"

char *make_indent(size_t indent_depth) {
  if (indent_depth == 0)
    return strdup("");

  char *result = malloc((sizeof(ONE_INDENT) - 1) * indent_depth + 1);
  if (result == NULL)
    err_malloc_fail();

  char *ptr = result;
  for (size_t i = 0; i < indent_depth; ++i) {
    memcpy(ptr, ONE_INDENT, sizeof(ONE_INDENT) - 1);
    ptr += sizeof(ONE_INDENT) - 1;
  }
  *ptr = '\0';

  return result;
}

void start_parsing(const char *node_name, size_t indent_depth) {
  char *indent = make_indent(indent_depth);
  printf("%s<%s>\n", indent, node_name);
  free(indent);
}

void finish_parsing(const char *node_name, size_t indent_depth) {
  char *indent = make_indent(indent_depth);
  printf("%s</%s>\n", indent, node_name);
  free(indent);
}

/**
 * @brief Used to debug self closing nodes, e.g. nodes with no children.
 */
void start_and_finish_parsing(const char *node_name, size_t indent_depth) {
  char *indent = make_indent(indent_depth);
  printf("%s<%s />\n", indent, node_name);
  free(indent);
}
