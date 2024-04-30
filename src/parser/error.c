#include <stdio.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "parser/error.h"
#include "parser/utils.h"

/**
 * @brief Prints the current position in the lexer. Should print something like:
 * ```
 * some_file.keb:1:21:
 * def some-wrong-code 3
 *                     ^
 * ```
 *
 * @param lexer lexer to print position of
 */
static void print_lexer_pos(lexer_t *lexer) {
  // NOTE: lexer->line should always have a newline
  fprintf(stderr, "%s:%zu:%zu\n", lexer->source_file->f_name,
          lexer->line_number, lexer->prev_pos);
  fprintf(stderr, "%s%s^\n", lexer->line, repeat_char(' ', lexer->prev_pos));
}

/**
 * @brief Prints a line from a start position to the current position in the
 * lexer. Should print something like:
 * ```
 * some_file.keb:1:21:
 * some-wrong-code int(2)
 *                 ^~~~~
 * ```
 *
 * @param start_pos where to start printing line from.
 * @param lexer lexer to print position of
 */
static void print_lexer_pos_from(lexer_t *lexer, size_t start_pos) {
  fprintf(stderr, "%s:%zu:%zu\n", lexer->source_file->f_name,
          lexer->line_number, lexer->prev_pos);
  fprintf(stderr, "%s%s^%s\n", lexer->line, repeat_char(' ', start_pos),
          repeat_char('~', lexer->prev_pos - start_pos));
}

_Noreturn void err_illegal_statement(lexer_t *lexer, size_t stmt_start) {
  print_lexer_pos_from(lexer, stmt_start);
  fprintf(stderr, "illegal-statement-error\n");
  exit(1);
}

_Noreturn void err_illegal_token(lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "illegal-token-error: '%s'\n",
          token_to_string(lexer->cur_token));
  exit(1);
}

_Noreturn void err_wrong_token(lexer_t *lexer, const char *expected) {
  print_lexer_pos(lexer);
  fprintf(stderr, "wrong-token-error: expected '%s', got '%s'\n", expected,
          token_to_string(lexer->cur_token));

  exit(1);
}

_Noreturn void err_missing_return(lexer_t *lexer) {
  print_lexer_pos(lexer);
  // TODO: trace the type of the primitive constructor here
  fprintf(stderr,
          "missing-return-error: constructors must end with an expression\n");
  exit(1);
}
