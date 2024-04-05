#include <stddef.h>

#define ONE_INDENT "  "

char *make_indent(size_t indent_depth);
void start_parsing(const char *node_name, size_t indent_depth);
void finish_parsing(const char *node_name, size_t indent_depth);
void start_and_finish_parsing(const char *node_name, size_t indent_depth);
