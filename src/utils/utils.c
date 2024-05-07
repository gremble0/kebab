#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nerror.h"
#include "utils/utils.h"

/**
 * @brief Creates a malloc'd string consisting of the character `c` repeated `n`
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
