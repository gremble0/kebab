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
    size_t res_size = sizeof("<char-literal=' '>");
    char res[res_size];
    sprintf(res, "char-literal: '%c'", token->char_literal);
    return string_of(res, res_size);
  }
  case TOKEN_STRING_LITERAL: {
    size_t res_size = sizeof("<string-literal=\"\">") + token->string_literal->len;
    char res[res_size];
    sprintf(res, "<string-literal=\"%.*s\">", (int)token->string_literal->len,
            token->string_literal->s);
    return string_of(res, res_size);
  }
  case TOKEN_INT_LITERAL: {
    size_t res_size = snprintf(NULL, 0, "%ld", token->integer_literal) + sizeof("<int-literal=");
    char res[res_size];
    sprintf(res, "<int-literal=%ld>", token->integer_literal);
    return string_of(res, res_size);
  }
  case TOKEN_NAME: {
    size_t res_size = sizeof("<name=\"\">") + token->name->len;
    char res[res_size];
    sprintf(res, "<name=\"%.*s\">", (int)token->name->len, token->name->s);
    return string_of(res, res_size);
  }
  default:
    // Other tokens are just constant strings that dont depend on the contents of the token
    return token_kind_to_string(token->kind);
  }
}

string_t *token_kind_to_string(token_kind_t kind) {
  switch (kind) {
  case TOKEN_DEF:
    return string_copy(&token_string_def);
  case TOKEN_SET:
    return string_copy(&token_string_set);
  case TOKEN_MUT:
    return string_copy(&token_string_mut);
  case TOKEN_NIL:
    return string_copy(&token_string_nil);
  case TOKEN_IF:
    return string_copy(&token_string_if);
  case TOKEN_ELIF:
    return string_copy(&token_string_elif);
  case TOKEN_ELSE:
    return string_copy(&token_string_else);
  case TOKEN_CHAR:
    return string_copy(&token_string_char);
  case TOKEN_STRING:
    return string_copy(&token_string_string);
  case TOKEN_INT:
    return string_copy(&token_string_int);
  case TOKEN_BOOL:
    return string_copy(&token_string_bool);
  case TOKEN_FN:
    return string_copy(&token_string_fn);
  case TOKEN_LIST:
    return string_copy(&token_string_list);
  case TOKEN_TRUE:
    return string_copy(&token_string_true);
  case TOKEN_FALSE:
    return string_copy(&token_string_false);
  case TOKEN_COLON:
    return string_copy(&token_string_colon);
  case TOKEN_EQUALS:
    return string_copy(&token_string_equals);
  case TOKEN_COMMA:
    return string_copy(&token_string_comma);
  case TOKEN_LPAREN:
    return string_copy(&token_string_lparen);
  case TOKEN_RPAREN:
    return string_copy(&token_string_rparen);
  case TOKEN_LBRACE:
    return string_copy(&token_string_lbrace);
  case TOKEN_RBRACE:
    return string_copy(&token_string_rbrace);
  case TOKEN_FAT_RARROW:
    return string_copy(&token_string_fat_rarrow);
  case TOKEN_PLUS:
    return string_copy(&token_string_plus);
  case TOKEN_MINUS:
    return string_copy(&token_string_minus);
  case TOKEN_MULT:
    return string_copy(&token_string_mult);
  case TOKEN_DIV:
    return string_copy(&token_string_div);
  case TOKEN_NOT:
    return string_copy(&token_string_not);
  case TOKEN_LT:
    return string_copy(&token_string_lt);
  case TOKEN_LE:
    return string_copy(&token_string_le);
  case TOKEN_EQ:
    return string_copy(&token_string_eq);
  case TOKEN_NEQ:
    return string_copy(&token_string_neq);
  case TOKEN_GT:
    return string_copy(&token_string_gt);
  case TOKEN_GE:
    return string_copy(&token_string_ge);
  case TOKEN_EOF:
    return string_copy(&token_string_eof);
  case TOKEN_CHAR_LITERAL:
    return string_copy(&token_string_char_lit);
  case TOKEN_STRING_LITERAL:
    return string_copy(&token_string_string_lit);
  case TOKEN_INT_LITERAL:
    return string_copy(&token_string_int_lit);
  case TOKEN_NAME:
    return string_copy(&token_string_name);
  }
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

  token->kind = TOKEN_INT_LITERAL;
  token->integer_literal = int_lit;
  token->span = span;

  return token;
}
