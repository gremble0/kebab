#include <stdio.h>
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

/**
 * @brief Get the string of the given line from a file. Needs to be freed by
 * caller
 *
 * @param f file to get line number from
 * @param line_number line number to get from file, first line is 1, calling
 * with 0 returns NULL
 * @return A mallod'cd string of the given line from the file
 */
char *get_line_from_file(FILE *f, size_t line_number) {
  if (f == NULL)
    return NULL;

  char *line = NULL;
  size_t len = 0;

  // Rewind the file to the beginning
  rewind(f);

  while (1) {
    ssize_t read = getline(&line, &len, f);

    // If we have reached our line or an unexpected EOF, return the current line
    if (line_number-- <= 1 || read == -1)
      return line;

    free(line);
    line = NULL;
  }
}
