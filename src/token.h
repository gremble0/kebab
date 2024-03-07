#pragma once

typedef struct token_t {
  enum token_kind_t {
    // Keywords
    TOKEN_DEF,
    TOKEN_MUT,

    // Constructors
    // TODO: These should be defined by parser or even in language stdlib
    // TOKEN_INT,
    // TOKEN_STRING,
    TOKEN_FN,

    // Syntax
    TOKEN_COLON,      // :
    TOKEN_EQUALS,     // =
    TOKEN_COMMA,      // ,
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_LBRACE,     // [
    TOKEN_RBRACE,     // ]
    TOKEN_FAT_RARROW, // =>

    // Primitives
    TOKEN_PLUS,  // +
    TOKEN_MINUS, // -
    TOKEN_MULT,  // *
    TOKEN_DIV,   // /

    // Values
    TOKEN_CHAR_LITERAL,
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
    char char_literal; // TODO: int for unicode or something? idk
    int integer_literal;
  };
} token_t;

void token_print(token_t *token);
void token_free(token_t *token);

token_t *token_make_simple(enum token_kind_t token_kind);
token_t *token_make_str_lit(const char *str_lit);
token_t *token_make_char_lit(const char char_lit);
token_t *token_make_int_lit(int int_lit);
token_t *token_make_name(const char *name);
