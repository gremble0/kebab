#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

/**
 * @brief converts a token to a string for debugging information
 *
 * @param token token to stringify
 */
char *token_to_string(token_t *token) {
  switch (token->kind) {
  case TOKEN_DEF:
  case TOKEN_SET:
  case TOKEN_MUT:
  case TOKEN_FN:
  case TOKEN_COLON:
  case TOKEN_EQUALS:
  case TOKEN_COMMA:
  case TOKEN_LPAREN:
  case TOKEN_RPAREN:
  case TOKEN_LBRACE:
  case TOKEN_RBRACE:
  case TOKEN_FAT_RARROW:
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_EOL:
  case TOKEN_EOF: {
    // Maps simple tokens to its stringified version. Indexing this array
    // with a tokens kind will return its string version
    static const char *simple_tokens[] = {
        "def ", "set ", "mut ", "fn", ":", "=", ",", "(",  ")",
        "[",    "]",    "=>",   "+",  "-", "*", "/", "\n", ""};
    return strdup(simple_tokens[token->kind]);
  }
  case TOKEN_CHAR_LITERAL: {
    char *res = malloc(4); // 3 for 'c' and 1 for NULL terminator
    sprintf(res, "'%c'", token->char_literal);
    return res;
  }
  case TOKEN_STRING_LITERAL: {
    // 2 for quotes and 1 for NULL terminator
    char *res = malloc(strlen(token->string_literal) + 3);
    sprintf(res, "\"%s\"", token->string_literal);
    return res;
  }
  case TOKEN_INTEGER_LITERAL: {
    int len = snprintf(NULL, 0, "%d", token->integer_literal);
    char *res = malloc(len + 1);
    sprintf(res, "%d", token->integer_literal);
    return res;
  }
  case TOKEN_NAME: {
    char *res = malloc(strlen(token->name) + 1);
    sprintf(res, "%s", token->name);
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
  token_t *token = malloc(sizeof(token_t));
  if (token == NULL) {
    err_malloc_fail();
  }
  token->kind = token_kind;

  return token;
}

token_t *token_make_str_lit(const char *str_lit) {
  token_t *str_lit_tok = malloc(sizeof(token_t));
  if (str_lit_tok == NULL) {
    err_malloc_fail();
  }
  str_lit_tok->kind = TOKEN_STRING_LITERAL;
  str_lit_tok->string_literal = str_lit;

  return str_lit_tok;
}

token_t *token_make_char_lit(char char_lit) {
  token_t *char_lit_tok = malloc(sizeof(token_t));
  if (char_lit_tok == NULL) {
    err_malloc_fail();
  }
  char_lit_tok->kind = TOKEN_CHAR_LITERAL;
  char_lit_tok->char_literal = char_lit;

  return char_lit_tok;
}

token_t *token_make_int_lit(int int_lit) {
  token_t *int_lit_tok = malloc(sizeof(token_t));
  if (int_lit_tok == NULL) {
    err_malloc_fail();
  }
  int_lit_tok->kind = TOKEN_INTEGER_LITERAL;
  int_lit_tok->integer_literal = int_lit;

  return int_lit_tok;
}

token_t *token_make_name(const char *name) {
  token_t *name_tok = malloc(sizeof(token_t));
  if (name_tok == NULL) {
    err_malloc_fail();
  }
  name_tok->kind = TOKEN_NAME;
  name_tok->name = name;

  return name_tok;
}

_Noreturn void err_illegal_token(token_t *token) {
  err_print_stacktrace();
  fprintf(stderr, "ERR_ILLEGAL_TOKEN: '%s'", token_to_string(token));
  exit(1);
}
