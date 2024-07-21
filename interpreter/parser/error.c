#include <stdio.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "nonstdlib/nstring.h"
#include "parser/error.h"
#include "utils/utils.h"

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
  string_t *indent = repeat_char(' ', lexer->prev_line_pos);

  fprintf(stderr, "%s:%zu:%zu\n", lexer->file.name, lexer->line_number, lexer->prev_line_pos);
  fprintf(stderr, "%.*s%.*s^\n", (int)lexer->line->len, lexer->line->s, (int)indent->len,
          indent->s);

  string_free(indent);
}

// TODO: const char *context? statement, expression, etc.
// TODO: also ideally none of these functions take the lexer as a param
_Noreturn void err_illegal_token(lexer_t *lexer) {
  print_lexer_pos(lexer);
  fprintf(stderr, "illegal-token-error: '%s'\n", token_to_string(lexer->cur_token)->s);
  exit(1);
}

_Noreturn void err_wrong_token(lexer_t *lexer, string_t *expected, string_t *actual) {
  print_lexer_pos(lexer);
  fprintf(stderr, "wrong-token-error: expected '%.*s', got '%.*s'\n", (int)expected->len,
          expected->s, (int)actual->len, actual->s);

  exit(1);
}

_Noreturn void err_missing_return(lexer_t *lexer) {
  print_lexer_pos(lexer);
  // TODO: trace the type of the primitive constructor here
  fprintf(stderr, "missing-return-error: constructors must end with an expression\n");
  exit(1);
}
