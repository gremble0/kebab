#include "lexer/lexer.h"

_Noreturn void err_illegal_token(lexer_t *lexer);
_Noreturn void err_wrong_token(const char *expected, lexer_t *lexer);
