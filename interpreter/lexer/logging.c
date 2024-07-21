#include <stdio.h>

#include "lexer/logging.h"

/**
 * @brief Write a token into the log file
 *
 * @param token token to log
 */
void lexer_log_token(const token_t *token) {
  string_t *token_string = token_to_string(token);
  fprintf(log_file, "%.*s [%zu, %zu] - [%zu, %zu]\n", (int)token_string->len,
          token_string->s, token->span.start.line, token->span.start.col,
          token->span.end.line, token->span.end.col);
  string_free(token_string);
}

/**
 * @brief Handle initializing lexer logging
 */
void lexer_log_start() { log_file = fopen("keb-lexer.log", "w"); }

/**
 * @brief Handle deinitializing lexer logging
 */
void lexer_log_finish() { fclose(log_file); }
