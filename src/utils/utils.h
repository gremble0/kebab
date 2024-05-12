#include <stddef.h>

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])
#define LIST_START_SIZE 8
#define TABLE_START_SIZE 8

char *repeat_char(char c, size_t n);
