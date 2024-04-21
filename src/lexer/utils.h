#pragma once

#include <stdio.h>

#include "lexer/token.h"

#ifdef DEBUG_LEXER
static FILE *log_file;
#define START_LEXING() start_lexing();
#define FINISH_LEXING() finish_lexing();
#define LOG_TOKEN(token) log_token(token);
#else
#define LOG_TOKEN(token)
#define START_LEXING()
#endif

void log_token(token_t *token);
void start_lexing();
void finish_lexing();
