#pragma once

#include <stddef.h>
#include <stdio.h>

#include "nonstdlib/nstring.h"

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])
#define LIST_START_SIZE 8
#define TABLE_START_SIZE 8
#define ONE_INDENT_WIDTH 2

// file.h & file.c ?
typedef struct {
  FILE *f;
  const char *name; // used for error handling
} file_t;

typedef struct {
  size_t line;
  size_t col;
} position_t;

typedef struct {
  position_t start;
  position_t end;
  file_t file;
} span_t;

#define span_of(start_line, start_col, end_line, end_col, file)                                    \
  (span_t) { (position_t){start_line, start_col}, (position_t){end_line, end_col}, file }

string_t *repeat_char(char c, size_t n);
string_t *get_line_from_file(FILE *f, size_t line_number);
