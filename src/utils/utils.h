#pragma once

#include <stddef.h>
#include <stdio.h>

#include "nonstdlib/nstring.h"

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])
#define LIST_START_SIZE 8
#define TABLE_START_SIZE 8

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

string_t *repeat_char(char c, size_t n);
char *get_line_from_file(FILE *f, size_t line_number);
