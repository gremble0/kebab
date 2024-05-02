#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nerror.h"
#include "parser/utils.h"

void parser_log_start() {
  log_file = fopen("keb-parser.log", "w");
  parser_log_node_start("ast");
}

void parser_log_finish() {
  parser_log_node_finish("ast");
  fclose(log_file);
}

/**
 * @brief Log opening of a node to the logfile - something like `<node>`
 *
 * @param node_name name of node to close
 */
void parser_log_node_start(const char *node_name, ...) {
  char *indent = repeat_char(' ', ONE_INDENT_WIDTH * indent_depth);

  va_list args;
  va_start(args, node_name);
  fprintf(log_file, "%s<", indent);
  vfprintf(log_file, node_name, args);
  fprintf(log_file, ">\n");
  va_end(args);

  free(indent);
  ++indent_depth;
}

/**
 * @brief Log closing of a node - something like `</node>`
 *
 * @param node_name name of node to close
 */
void parser_log_node_finish(const char *node_name, ...) {
  --indent_depth;
  char *indent = repeat_char(' ', ONE_INDENT_WIDTH * indent_depth);

  va_list args;
  va_start(args, node_name);
  fprintf(log_file, "%s<", indent);
  vfprintf(log_file, node_name, args);
  fprintf(log_file, "/>\n");
  va_end(args);

  free(indent);
}

/**
 * @brief Log a self closing node to the log file - something like
 * `<node='c' />`
 */
void parser_log_node_self_closing(const char *node_name, ...) {
  char *indent = repeat_char(' ', ONE_INDENT_WIDTH * indent_depth);

  va_list args;
  va_start(args, node_name);
  fprintf(log_file, "%s<", indent);
  vfprintf(log_file, node_name, args);
  fprintf(log_file, " />\n");
  va_end(args);

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
  if (str == NULL)
    err_malloc_fail();

  memset(str, c, n);
  str[n] = '\0';

  return str;
}
