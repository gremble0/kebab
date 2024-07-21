#pragma once

#include <stdio.h>

#include "lexer/token.h"

#ifdef DEBUG_LEXER
#define LEXER_LOG_START() lexer_log_start()
#define LEXER_LOG_FINISH() lexer_log_finish()
#define LEXER_LOG_TOKEN(token) lexer_log_token(token)
#else
#define LEXER_LOG_START()
#define LEXER_LOG_FINISH()
#define LEXER_LOG_TOKEN(token)
#endif

static FILE *log_file;

void lexer_log_token(const token_t *token);
void lexer_log_start();
void lexer_log_finish();
