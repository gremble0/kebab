#include <stdio.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "parser/error.h"
#include "parser/utils.h"

/**
 * @brief Prints the current position in the lexer. Should print something like:
 * """some_file.keb:1:21:
 *    def some-wrong-code 3
 *                        ^"""
 *
 * @param lexer lexer to get error position from.
 */
static void print_lexer_pos(lexer_t *lexer) {
  // NOTE: lexer->line should always have a newline
  fprintf(stderr, "%s:%zu:%zu\n", lexer->source_file->f_name,
          lexer->line_number, lexer->prev_pos);
  fprintf(stderr, "%s%s^\n", lexer->line, repeat_char(' ', lexer->prev_pos));
}

_Noreturn void err_wrong_token(const char *expected, lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "ERR_WRONG_TOKEN: expected '%s', got '%s'\n", expected,
          token_to_string(lexer->cur_token));

  exit(1);
}

_Noreturn void err_illegal_token(lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: '%s'\n",
          token_to_string(lexer->cur_token));
  exit(1);
}
