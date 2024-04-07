#include "lexer.h"

_Noreturn void err_illegal_token(lexer_t *lexer);
_Noreturn void err_wrong_token(token_kind_t expected_kind, lexer_t *lexer);
