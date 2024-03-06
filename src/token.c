#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "token.h"

/**
 * @brief Debugging function to print the "toString" of a token
 *
 * @param token token to print
 */
void token_print(token_t *token) {
  switch (token->kind) {
  case TOKEN_DEF: {
    puts("TOKEN_DEF");
  } break;
  case TOKEN_INT: {
    puts("TOKEN_INT");
  } break;
  case TOKEN_FN: {
    puts("TOKEN_FN");
  } break;
  case TOKEN_COLON: {
    puts("TOKEN_COLON");
  } break;
  case TOKEN_EQUALS: {
    puts("TOKEN_EQUALS");
  } break;
  case TOKEN_COMMA: {
    puts("TOKEN_COMMA");
  } break;
  case TOKEN_LBRACE: {
    puts("TOKEN_LBRACE");
  } break;
  case TOKEN_RBRACE: {
    puts("TOKEN_RBRACE");
  } break;
  case TOKEN_FAT_RARROW: {
    puts("TOKEN_FAT_RARROW");
  } break;
  case TOKEN_PLUS: {
    puts("TOKEN_PLUS");
  } break;
  case TOKEN_MINUS: {
    puts("TOKEN_MINUS");
  } break;
  case TOKEN_MULT: {
    puts("TOKEN_MULT");
  } break;
  case TOKEN_DIV: {
    puts("TOKEN_DIV");
  } break;
  case TOKEN_STRING_LITERAL: {
    printf("TOKEN_STRING_LITERAL: \"%s\"\n", token->string_literal);
  } break;
  case TOKEN_INTEGER_LITERAL: {
    printf("TOKEN_INTEGER_LITERAL: %d\n", token->integer_literal);
  } break;
  case TOKEN_NAME: {
    printf("TOKEN_NAME: '%s'\n", token->name);
  } break;
  case TOKEN_EOL: {
    puts("TOKEN_EOL");
  } break;
  case TOKEN_EOF: {
  } break;
  }
}

token_t *token_make_def() {
  token_t *def_token = malloc(sizeof(token_t));
  if (def_token == NULL) {
    err_malloc_fail();
  }
  def_token->kind = TOKEN_DEF;

  return def_token;
}

token_t *token_make_colon() { return NULL; }

token_t *token_make_equals() { return NULL; }

token_t *token_make_comma() { return NULL; }

token_t *token_make_lbrace() { return NULL; }

token_t *token_make_rbrace() { return NULL; }

token_t *token_make_fat_rarrow() { return NULL; }

token_t *token_make_plus() {
  token_t *plus = malloc(sizeof(token_t));
  if (plus == NULL) {
    err_malloc_fail();
  }
  plus->kind = TOKEN_PLUS;

  return plus;
}

token_t *token_make_minus() { return NULL; }

token_t *token_make_mult() { return NULL; }

token_t *token_make_div() { return NULL; }

token_t *token_make_str_lit(const char *str_lit) {
  token_t *str_lit_tok = malloc(sizeof(token_t));
  if (str_lit_tok == NULL) {
    err_malloc_fail();
  }
  str_lit_tok->kind = TOKEN_STRING_LITERAL;
  str_lit_tok->string_literal = str_lit;

  return str_lit_tok;
}

token_t *token_make_int_lit(const int int_lit) { return NULL; }
token_t *token_make_name(const char *name) {
  token_t *name_tok = malloc(sizeof(token_t));
  if (name_tok == NULL) {
    err_malloc_fail();
  }
  name_tok->kind = TOKEN_NAME;
  name_tok->name = name;

  return name_tok;
}

token_t *token_make_eol() {
  token_t *eol_tok = malloc(sizeof(token_t));
  if (eol_tok == NULL) {
    err_malloc_fail();
  }
  eol_tok->kind = TOKEN_EOL;

  return eol_tok;
}

token_t *token_make_eof() {
  token_t *eof = malloc(sizeof(token_t));
  if (eof == NULL) {
    err_malloc_fail();
  }
  eof->kind = TOKEN_EOF;

  return eof;
}
