#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nstring.h"

string_t *token_to_string(const token_t *token) {
  switch (token->kind) {
  case TOKEN_CHAR_LITERAL: {
    size_t res_size = sizeof("char-literal: ' '");
    char res[res_size];
    sprintf(res, "char-literal: '%c'", token->char_literal);
    return string_of(res, res_size);
  }
  case TOKEN_STRING_LITERAL: {
    size_t res_size =
        sizeof("string-literal: \"\"") + token->string_literal->len;
    char res[res_size];
    sprintf(res, "string-literal: \"%.*s\"", (int)token->string_literal->len,
            token->string_literal->s);
    return string_of(res, res_size);
  }
  case TOKEN_INTEGER_LITERAL: {
    size_t res_size = snprintf(NULL, 0, "%ld", token->integer_literal) +
                      sizeof("integer-literal: ");
    char res[res_size];
    sprintf(res, "integer-literal: %ld", token->integer_literal);
    return string_of(res, res_size);
  }
  case TOKEN_NAME: {
    size_t res_size = sizeof("name: \"\"") + token->name->len;
    char res[res_size];
    sprintf(res, "name: \"%.*s\"", (int)token->name->len, token->name->s);
    return string_of(res, res_size);
  }
  default:
    return token_kind_to_string(token->kind);
  }
}

string_t *token_kind_to_string(token_kind_t kind) {
  return string_copy(token_kind_map[kind]);
}

void token_free(token_t *token) {
  switch (token->kind) {
  case TOKEN_NAME:
    string_free(token->name);
    free(token);
    break;
  case TOKEN_STRING_LITERAL:
    string_free(token->string_literal);
    free(token);
    break;
  default:
    free(token);
  }
}

/**
 * @brief Make a token with no special metadata on it based on a token kind
 *
 * @param token_kind what to make a token from
 * @return a malloc'd token with the appropriate token_kind
 */
token_t *token_make_simple(token_kind_t token_kind, span_t span) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL)
    err_malloc_fail();

  token->kind = token_kind;
  token->span = span;

  return token;
}

token_t *token_make_name(string_t *name, span_t span) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL)
    err_malloc_fail();

  token->kind = TOKEN_NAME;
  token->name = name;
  token->span = span;

  return token;
}

token_t *token_make_str_lit(string_t *str_lit, span_t span) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL)
    err_malloc_fail();

  token->kind = TOKEN_STRING_LITERAL;
  token->string_literal = str_lit;
  token->span = span;

  return token;
}

token_t *token_make_char_lit(uint8_t char_lit, span_t span) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL)
    err_malloc_fail();

  token->kind = TOKEN_CHAR_LITERAL;
  token->char_literal = char_lit;
  token->span = span;

  return token;
}

token_t *token_make_int_lit(int64_t int_lit, span_t span) {
  token_t *token = malloc(sizeof(*token));
  if (token == NULL)
    err_malloc_fail();

  token->kind = TOKEN_INTEGER_LITERAL;
  token->integer_literal = int_lit;
  token->span = span;

  return token;
}
