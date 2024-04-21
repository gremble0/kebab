#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/utils.h"

/**
 * @brief Write a token into the log file
 *
 * @param token token to log
 */
void log_token(token_t *token) {
  char *token_string = token_to_string(token);
  int token_string_len = strlen(token_string);
  fwrite(token_string, 1, token_string_len, log_file);
  fwrite("\n", 1, 1, log_file);
  free(token_string);
}

void start_lexing() { log_file = fopen("keb-lexer.log", "w"); }
void finish_lexing() { fclose(log_file); }
