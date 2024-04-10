#include <stdio.h>
#include <stdlib.h>

#include "parser_error.h"
#include "token.h"
#include "utils.h"

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

_Noreturn void err_wrong_token(token_kind_t expected_kind, lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "ERR_WRONG_TOKEN: expected '%s', got '%s'\n",
          token_kind_to_string(expected_kind),
          token_to_string(lexer->cur_token));

  exit(1);
}

_Noreturn void err_illegal_token(lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: '%s'\n",
          token_to_string(lexer->cur_token));
  exit(1);
}

// TODO: this is a runtime error
_Noreturn void err_set_constant(lexer_t *lexer, token_t *constant) {
  print_lexer_pos(lexer);
  fprintf(stderr,
          "ERR_SET_CONSTANT: cannot mutate '%s' with 'set'. Define variable "
          "with 'mut'\n",
          token_to_string(constant));
  exit(1);
}
