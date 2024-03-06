#include <stdio.h>

typedef struct token_t {
  enum kind {
    // Definitions
    TOKEN_DEF,

    // Constructors
    TOKEN_INT,
    TOKEN_FN,

    // Syntax
    TOKEN_COLON,      // :
    TOKEN_EQUALS,     // =
    TOKEN_COMMA,      // ,
    TOKEN_LBRACE,     // [
    TOKEN_RBRACE,     // ]
    TOKEN_FAT_RARROW, // =>

    // Primitives
    TOKEN_PLUS,  // +
    TOKEN_MINUS, // -
    TOKEN_MULT,  // *
    TOKEN_DIV,   // /

    // Values
    TOKEN_STRING_LITERAL,
    TOKEN_INTEGER_LITERAL,
    // TODO: TOKEN_FLOAT_LITERAL,
    TOKEN_NAME,

    TOKEN_EOL, // \n | \r
    TOKEN_EOF, // <end of file>
  } kind;
  union {
    const char *name;
    const char *string_literal;
    int integer_literal;
  };
} token_t;

#define LEXER_BUF_SIZE 256
typedef struct lexer_t {
  FILE *source_file;
  char *line;
  ssize_t line_len;
  size_t line_pos;
} lexer_t;

lexer_t *lexer_init(const char *file_path);
token_t *lexer_next_token(lexer_t *lexer);
void lexer_free(lexer_t *lexer);
