#include <stdio.h>

typedef struct token_t {
  enum kind {
    // Definitions
    TOKEN_DEFP,
    TOKEN_DEFV,

    // Syntax
    TOKEN_COLON,      // :
    TOKEN_EQUALS,     // =
    TOKEN_COMMA,      // ,
    TOKEN_LBRACE,     // ]
    TOKEN_RBRACE,     // [
    TOKEN_FAT_RARROW, // =>

    // Primitives
    TOKEN_PLUS,  // +
    TOKEN_MINUS, // -
    TOKEN_MULT,  // *
    TOKEN_DIV,   // /

    // Values
    TOKEN_STRING_LITERAL,
    TOKEN_INTEGER_LITERAL,
    TOKEN_NAME,
    // TODO: TOKEN_FLOAT_LITERAL,
  } kind;
  union {
    char *name;
    char *string_literal;
    int integer_literal;
  };
} token_t;

typedef struct lexer_t {
  FILE *source_file;
  char current;
} lexer_t;

lexer_t *lexer_init(const char *file_path);
void lexer_free(lexer_t *lexer);
token_t *lexer_next_token(lexer_t *lexer);
