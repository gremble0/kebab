#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "lexer.h"

/**
 * @brief Debugging function to print the "toString" of a token
 *
 * @param token token to print
 */
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

/**
 * @brief Loads the LEXER_BUF_SIZE next bytes into the lexers buffer for reading
 * the source file, also resets buffer_pos
 *
 * @param lexer lexer to load next chunk for
 */
static void lexer_load_next_chunk(lexer_t *lexer) {
  fread(lexer->buffer, LEXER_BUF_SIZE, 1, lexer->source_file);
  lexer->buffer_pos = 0;
}

/**
 * @brief Builds up an int from the contents in the lexers buffer, loading new
 * chunks if necessary
 *
 * @param lexer lexer to read int from
 * @return the next number in the lexer
 */
static const int lexer_read_int(lexer_t *lexer) {
  char c = lexer->buffer[lexer->buffer_pos];

  return 0;
}

/**
 * @brief Builds up a string from the contents the lexers buffer, loading new
 * chunks if necessary
 *
 * @param lexer lexer to read string from
 * @return the next string in the lexer
 */
static const char *lexer_read_str(lexer_t *lexer) {
  // TODO: implement
  return 0;
}

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
  lexer->source_file = f;
  lexer->buffer_pos = 0;
  lexer_load_next_chunk(lexer);

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

  printf("%zu: %s\n", lexer->buffer_pos, lexer->buffer);
  if (lexer->buffer_pos == LEXER_BUF_SIZE - 1) {
    lexer_load_next_chunk(lexer);
  }

  switch (lexer->buffer[lexer->buffer_pos]) {
  case ':': {
    next_token->kind = TOKEN_COLON;
  } break;
  case '=': {
    next_token->kind = TOKEN_EQUALS;
  } break;
  case ',': {
    next_token->kind = TOKEN_COMMA;
  } break;
  case '[': {
    next_token->kind = TOKEN_LBRACE;
  } break;
  case ']': {
    next_token->kind = TOKEN_RBRACE;
  } break;
  case '+': {
    next_token->kind = TOKEN_PLUS;
  } break;
  case '-': {
    next_token->kind = TOKEN_MINUS;
  } break;
  case '*':
    next_token->kind = TOKEN_MULT;
    break;
  case '/':
    next_token->kind = TOKEN_DIV;
    break;

  // Numbers can only start with 0 if they are 0, eg. 01 is an error
  case '0': {
    next_token->kind = TOKEN_INTEGER_LITERAL;
    next_token->integer_literal = 0;
  } break;
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    const int num = lexer_read_int(lexer);
    next_token->kind = TOKEN_INTEGER_LITERAL;
    next_token->integer_literal = num;
  } break;

  case '"':
  case '\'': {
    const char *str = lexer_read_str(lexer);
    next_token->kind = TOKEN_STRING_LITERAL;
    next_token->string_literal = str;
  } break;

  default: {
    const char *str = lexer_read_str(lexer);
    next_token->kind = TOKEN_NAME;
    next_token->name = str;
  } break;
  }

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
