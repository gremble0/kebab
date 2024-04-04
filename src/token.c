#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nerror.h"
#include "token.h"

/**
 * @brief converts a token to a string for debugging information
 *
 * @param token token to stringify
 */
char *token_to_string(token_t *token) {
  switch (token->kind) {
  case TOKEN_DEF:
    return strdup("TOKEN_DEF");
  case TOKEN_SET:
    return strdup("TOKEN_SET");
  case TOKEN_MUT:
    return strdup("TOKEN_MUT");
  case TOKEN_INT:
    return strdup("TOKEN_INT");
  case TOKEN_STRING:
    return strdup("TOKEN_STRING");
  case TOKEN_FN:
    return strdup("TOKEN_FN");
  case TOKEN_COLON:
    return strdup("TOKEN_COLON");
  case TOKEN_EQUALS:
    return strdup("TOKEN_EQUALS");
  case TOKEN_COMMA:
    return strdup("TOKEN_COMMA");
  case TOKEN_LPAREN:
    return strdup("TOKEN_LPAREN");
  case TOKEN_RPAREN:
    return strdup("TOKEN_RPAREN");
  case TOKEN_LBRACE:
    return strdup("TOKEN_LBRACE");
  case TOKEN_RBRACE:
    return strdup("TOKEN_RBRACE");
  case TOKEN_FAT_RARROW:
    return strdup("TOKEN_FAT_RARROW");
  case TOKEN_PLUS:
    return strdup("TOKEN_PLUS");
  case TOKEN_MINUS:
    return strdup("TOKEN_MINUS");
  case TOKEN_MULT:
    return strdup("TOKEN_MULT");
  case TOKEN_DIV:
    return strdup("TOKEN_DIV");
  case TOKEN_NOT:
    return strdup("TOKEN_NOT");
  case TOKEN_TRUE:
    return strdup("TOKEN_TRUE");
  case TOKEN_FALSE:
    return strdup("TOKEN_FALSE");
  case TOKEN_EOL:
    return strdup("TOKEN_EOL");
  case TOKEN_EOF:
    return strdup("TOKEN_EOF");
  case TOKEN_CHAR_LITERAL: {
    char *res = malloc(sizeof("TOKEN_CHAR_LITERAL: 'c'") + 1);
    sprintf(res, "TOKEN_CHAR_LITERAL: '%c'", token->char_literal);
    return res;
  }
  case TOKEN_STRING_LITERAL: {
    char *res = malloc(sizeof("TOKEN_STRING_LITERAL: \"\"") +
                       strlen(token->string_literal) + 1);
    sprintf(res, "TOKEN_STRING_LITERAL: \"%s\"", token->string_literal);
    return res;
  }
  case TOKEN_INTEGER_LITERAL: {
    int len = snprintf(NULL, 0, "%d", token->integer_literal);
    char *res = malloc(sizeof("TOKEN_INTEGER_LITERAL: ") + len + 1);
    sprintf(res, "TOKEN_INTEGER_LITERAL: %d", token->integer_literal);
    return res;
  }
  case TOKEN_NAME: {
    char *res = malloc(sizeof("TOKEN_NAME: \"\"") + strlen(token->name) + 1);
    sprintf(res, "TOKEN_NAME: \"%s\"", token->name);
    return res;
  }
  }
}

void token_free(token_t *token) {
  switch (token->kind) {
  case TOKEN_NAME:
    free((void *)token->name);
    free(token);
    break;
  case TOKEN_STRING_LITERAL:
    free((void *)token->string_literal);
    free(token);
    break;
  default:
    free(token);
  }
}

token_t *token_make_simple(token_kind_t token_kind) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL) {
    err_malloc_fail();
  }
  token->kind = token_kind;

  return token;
}

token_t *token_make_str_lit(const char *str_lit) {
  token_t *str_lit_tok = malloc(sizeof(*str_lit_tok));
  if (str_lit_tok == NULL) {
    err_malloc_fail();
  }
  str_lit_tok->kind = TOKEN_STRING_LITERAL;
  str_lit_tok->string_literal = str_lit;

  return str_lit_tok;
}

token_t *token_make_char_lit(char char_lit) {
  token_t *char_lit_tok = malloc(sizeof(*char_lit_tok));
  if (char_lit_tok == NULL) {
    err_malloc_fail();
  }
  char_lit_tok->kind = TOKEN_CHAR_LITERAL;
  char_lit_tok->char_literal = char_lit;

  return char_lit_tok;
}

token_t *token_make_int_lit(int int_lit) {
  token_t *int_lit_tok = malloc(sizeof(*int_lit_tok));
  if (int_lit_tok == NULL) {
    err_malloc_fail();
  }
  int_lit_tok->kind = TOKEN_INTEGER_LITERAL;
  int_lit_tok->integer_literal = int_lit;

  return int_lit_tok;
}

token_t *token_make_name(const char *name) {
  token_t *name_tok = malloc(sizeof(*name_tok));
  if (name_tok == NULL) {
    err_malloc_fail();
  }
  name_tok->kind = TOKEN_NAME;
  name_tok->name = name;

  return name_tok;
}

_Noreturn void err_illegal_token(token_t *token) {
  err_print_stacktrace();
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: '%s'\n", token_to_string(token));
  exit(1);
}
