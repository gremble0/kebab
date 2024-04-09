#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nerror.h"
#include "utils.h"

static char *make_indent(size_t indent_depth) {
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
  char *indent = make_indent(indent_depth);
  printf("%s<%s>\n", indent, node_name);
  free(indent);
  ++indent_depth;
}

void finish_parsing(const char *node_name) {
  --indent_depth;
  char *indent = make_indent(indent_depth);
  printf("%s</%s>\n", indent, node_name);
  free(indent);
}

/**
 * @brief Used to debug self closing nodes, e.g. nodes with no children.
 */
void start_and_finish_parsing(const char *node_name) {
  char *indent = make_indent(indent_depth);
  printf("%s<%s />\n", indent, node_name);
  free(indent);
}
