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
  case TOKEN_LPAREN: {
    puts("TOKEN_LPAREN");
  } break;
  case TOKEN_RPAREN: {
    puts("TOKEN_RPAREN");
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

token_t *token_make_simple(enum token_kind_t token_kind) {
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

token_t *token_make_int_lit(const int int_lit) {
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
