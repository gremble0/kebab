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

    TOKEN_NAME,
  } kind;
} token_t;

typedef struct lexer_t {
  char *source;
  char current;
} lexer_t;

lexer_t *lexer_init();
char lexer_peek_char(lexer_t *lexer, int offset);
token_t *lexer_next(lexer_t *lexer);
