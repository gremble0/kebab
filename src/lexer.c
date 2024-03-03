#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "lexer.h"

static void print_token(token_t *token) {
  switch (token->kind) {
  case TOKEN_DEFP: {
    printf("defp ");
  } break;
  case TOKEN_DEFV: {
    printf("defv ");
  } break;
  case TOKEN_COLON: {
    printf(": ");
  } break;
  case TOKEN_EQUALS: {
    printf("= ");
  } break;
  case TOKEN_COMMA: {
    printf(", ");
  } break;
  case TOKEN_LBRACE: {
    printf("[");
  } break;
  case TOKEN_RBRACE: {
    printf("]");
  } break;
  case TOKEN_FAT_RARROW: {
    printf("=>");
  } break;
  case TOKEN_PLUS: {
    printf("+");
  } break;
  case TOKEN_MINUS: {
    printf("-");
  } break;
  case TOKEN_MULT: {
    printf("*");
  } break;
  case TOKEN_DIV: {
    printf("/");
  } break;
  case TOKEN_STRING_LITERAL: {
    printf("\"%s\n\" ", token->string_literal);
  } break;
  case TOKEN_INTEGER_LITERAL: {
    printf("%d ", token->integer_literal);
  } break;
  case TOKEN_NAME: {
    printf("%s ", token->name);
  } break;
  case TOKEN_EOL: {
    putchar('\n');
  } break;
  case TOKEN_EOF: {
  } break;
  }
}

static char lexer_peek_char(lexer_t *lexer, int offset);

/**
 * @brief Initialize a lexer, setting its inital state based on the file at
 * file_path. Returned lexer needs to be freed using lexer_free()
 *
 * @param file_path path to file to open lexer for
 * @return lexer at the start of the file at file_path
 */
lexer_t *lexer_init(const char *file_path) {
  lexer_t *lexer = malloc(sizeof(lexer_t));
  if (lexer == NULL) {
    err_malloc_fail();
  }

  FILE *f = fopen(file_path, "r");
  if (f == NULL) {
    err_io_fail(file_path);
  }

  return lexer;
}

/**
 * @brief Get the next token in the lexer, returned token needs to be freed
 *
 * @param lexer lexer to get token from
 * @return token with appropriate contents based on lexer position
 */
token_t *lexer_next_token(lexer_t *lexer) {
  token_t *next_token = malloc(sizeof(token_t));
  if (next_token == NULL) {
    err_malloc_fail();
  }

  // TODO: make token
  // print_token(next_token);

  next_token->kind = TOKEN_EOF;
  return next_token;
}

/**
 * @brief Free lexer and all data associated with it
 *
 * @param lexer lexer to free
 */
void lexer_free(lexer_t *lexer) {
  fclose(lexer->source_file);
  free(lexer);
}
