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
    return strdup("def");
  case TOKEN_SET:
    return strdup("set");
  case TOKEN_MUT:
    return strdup("mut");
  case TOKEN_CHAR:
    return strdup("char");
  case TOKEN_STRING:
    return strdup("string");
  case TOKEN_INT:
    return strdup("int");
  case TOKEN_FN:
    return strdup("fn");
  case TOKEN_LIST:
    return strdup("list");
  case TOKEN_COLON:
    return strdup(":");
  case TOKEN_EQUALS:
    return strdup("=");
  case TOKEN_COMMA:
    return strdup(",");
  case TOKEN_LPAREN:
    return strdup("(");
  case TOKEN_RPAREN:
    return strdup(")");
  case TOKEN_LBRACE:
    return strdup("[");
  case TOKEN_RBRACE:
    return strdup("]");
  case TOKEN_FAT_RARROW:
    return strdup("=>");
  case TOKEN_PLUS:
    return strdup("+");
  case TOKEN_MINUS:
    return strdup("-");
  case TOKEN_MULT:
    return strdup("*");
  case TOKEN_DIV:
    return strdup("/");
  case TOKEN_NOT:
    return strdup("~");
  case TOKEN_TRUE:
    return strdup("true");
  case TOKEN_FALSE:
    return strdup("false");
  case TOKEN_EOF:
    return strdup("<eof>");
  case TOKEN_CHAR_LITERAL: {
    char *res = malloc(2); // char and null term
    res[0] = token->char_literal;
    res[1] = '\0';
    return res;
  }
  case TOKEN_STRING_LITERAL: {
    return strdup(token->string_literal);
  }
  case TOKEN_INTEGER_LITERAL: {
    int len = snprintf(NULL, 0, "%d", token->integer_literal);
    char *res = malloc(len + 1);
    sprintf(res, "%d", token->integer_literal);
    return res;
  }
  case TOKEN_NAME: {
    return strdup(token->name);
  }
  }
}

char *token_kind_to_string(token_kind_t kind) {
  switch (kind) {
  case TOKEN_DEF:
    return strdup("def");
  case TOKEN_SET:
    return strdup("set");
  case TOKEN_MUT:
    return strdup("mut");
  case TOKEN_CHAR:
    return strdup("char");
  case TOKEN_STRING:
    return strdup("string");
  case TOKEN_INT:
    return strdup("int");
  case TOKEN_FN:
    return strdup("fn");
  case TOKEN_LIST:
    return strdup("list");
  case TOKEN_COLON:
    return strdup(":");
  case TOKEN_EQUALS:
    return strdup("=");
  case TOKEN_COMMA:
    return strdup(",");
  case TOKEN_LPAREN:
    return strdup("(");
  case TOKEN_RPAREN:
    return strdup(")");
  case TOKEN_LBRACE:
    return strdup("[");
  case TOKEN_RBRACE:
    return strdup("]");
  case TOKEN_FAT_RARROW:
    return strdup("=>");
  case TOKEN_PLUS:
    return strdup("+");
  case TOKEN_MINUS:
    return strdup("-");
  case TOKEN_MULT:
    return strdup("*");
  case TOKEN_DIV:
    return strdup("/");
  case TOKEN_NOT:
    return strdup("~");
  case TOKEN_TRUE:
    return strdup("true");
  case TOKEN_FALSE:
    return strdup("false");
  case TOKEN_EOF:
    return strdup("<eof>");
  case TOKEN_CHAR_LITERAL: {
    return strdup("<char literal>");
  }
  case TOKEN_STRING_LITERAL: {
    return strdup("<string literal>");
  }
  case TOKEN_INTEGER_LITERAL: {
    return strdup("<integer literal>");
  }
  case TOKEN_NAME: {
    return strdup("<name>");
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
