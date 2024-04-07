#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nerror.h"
#include "parser_error.h"
// TODO: recover filename for better error messages

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
  if (str == NULL) {
    err_malloc_fail();
  }

  memset(str, c, n);
  str[n] = '\0';

  return str;
}

/**
 * @brief Prints the current position in the lexer. Should print something like:
 * """def some-wrong-code 3
 *                        ^"""
 *
 * @param lexer lexer to get error position from.
 */
static void print_err_pos(lexer_t *lexer) {
  // NOTE: lexer->line should always have a newline
  fprintf(stderr, "%s%s^\n", lexer->line, repeat_char(' ', lexer->prev_pos));
}

_Noreturn void err_wrong_token(const char *expected, lexer_t *lexer) {
  fprintf(stderr, "%s:%zu: ERR_WRONG_TOKEN: expected '%s', got '%s'\n",
          lexer->source_file->f_name, lexer->line_number, expected,
          token_to_string(lexer->cur_token));
  print_err_pos(lexer);

  exit(1);
}

_Noreturn void err_illegal_token(lexer_t *lexer) {
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: %s'\n",
          token_to_string(lexer->cur_token));
  exit(1);
}

// TODO: this is a runtime error
_Noreturn void err_set_constant(token_t *constant) {
  fprintf(stderr,
          "ERR_SET_CONSTANT: cannot mutate '%s' with 'set'. Define variable "
          "with 'mut'\n",
          token_to_string(constant));
  exit(1);
}
