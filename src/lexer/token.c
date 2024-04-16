#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nerror.h"
#include "token.h"

char *token_to_string(token_t *token) {
  switch (token->kind) {
  case TOKEN_CHAR_LITERAL: {
    char *res = malloc(sizeof("char-literal: ' '") + 1);
    sprintf(res, "char-literal: '%c'", token->char_literal);
    return res;
  }
  case TOKEN_STRING_LITERAL: {
    char *res = malloc(sizeof("string-literal: \"\"") +
                       strlen(token->string_literal) + 1);
    sprintf(res, "string-literal: \"%s\"", token->string_literal);
    return res;
  }
  case TOKEN_INTEGER_LITERAL: {
    // Get length to malloc before actually writing to `res`
    int len = snprintf(NULL, 0, "%ld", token->integer_literal);
    char *res = malloc(sizeof("integer-literal: ") + len + 1);
    sprintf(res, "integer-literal: %ld", token->integer_literal);
    return res;
  }
  case TOKEN_NAME: {
    char *res = malloc(sizeof("name: \"\"") + strlen(token->name) + 1);
    sprintf(res, "name: \"%s\"", token->name);
    return res;
  }
  default:
    return token_kind_to_string(token->kind);
  }
}

char *token_kind_to_string(token_kind_t kind) {
  return strdup(token_kind_map[kind]);
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

token_t *token_make_int_lit(int64_t int_lit) {
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
