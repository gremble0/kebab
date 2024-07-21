#pragma once

#include "lexer/lexer.h"

_Noreturn void err_illegal_token(lexer_t *lexer);
_Noreturn void err_wrong_token(lexer_t *lexer, string_t *expected,
                               string_t *actual);
_Noreturn void err_missing_return(lexer_t *lexer);
