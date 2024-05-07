#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "parser/logging.h"
#include "utils/utils.h"

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
