#pragma once

#include <stddef.h>

#define ONE_INDENT "  "

static size_t indent_depth = 0;

void start_parsing(const char *node_name);
void finish_parsing(const char *node_name);
void start_and_finish_parsing(const char *node_name);
