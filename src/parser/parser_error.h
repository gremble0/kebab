#include "token.h"

_Noreturn void err_illegal_token(token_t *token);
_Noreturn void err_wrong_token(const char *expected, token_t *actual);
