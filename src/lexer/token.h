#pragma once

typedef enum {
  // Keywords
  TOKEN_DEF, // def
  TOKEN_SET, // set
  TOKEN_MUT, // mut
  TOKEN_NIL, // nil
  // TOKEN_DEC ? declare variable with type?
  // Constructors
  TOKEN_FN,   // fn
  TOKEN_LIST, // list

  NOT_A_KEYWORD, // NOT A VALID TOKEN (just used to compare with keywords)

  // Syntax
  TOKEN_COLON,      // :
  TOKEN_EQUALS,     // =
  TOKEN_COMMA,      // ,
  TOKEN_LPAREN,     // (
  TOKEN_RPAREN,     // )
  TOKEN_LBRACE,     // [
  TOKEN_RBRACE,     // ]
  TOKEN_FAT_RARROW, // =>

  // Operators
  TOKEN_PLUS,  // +
  TOKEN_MINUS, // -
  TOKEN_MULT,  // *
  TOKEN_DIV,   // /
  TOKEN_NOT,   // ~

  // Comparisons
  TOKEN_LT,  // <
  TOKEN_LE,  // <=
  TOKEN_EQ,  // ==
  TOKEN_NEQ, // ~=
  TOKEN_GT,  // >
  TOKEN_GE,  // >=

  // Booleans
  TOKEN_TRUE,  // true
  TOKEN_FALSE, // false

  // Special
  TOKEN_EOF, // <end of file>

  // Values
  TOKEN_CHAR_LITERAL,    // c
  TOKEN_STRING_LITERAL,  // string
  TOKEN_INTEGER_LITERAL, // 123
  // TODO: TOKEN_FLOAT_LITERAL,
  TOKEN_NAME,
} token_kind_t;

static const char *keywords[] = {
    [TOKEN_DEF] = "def", [TOKEN_SET] = "set", [TOKEN_MUT] = "mut",
    [TOKEN_NIL] = "nil", [TOKEN_FN] = "fn",   [TOKEN_LIST] = "list",
};

typedef struct {
  token_kind_t kind;
  union {
    const char *name;
    const char *string_literal;
    char char_literal; // TODO: int for unicode or something? idk
    int integer_literal;
  };
} token_t;

char *token_to_string(token_t *token);
char *token_kind_to_string(token_kind_t kind);
void token_free(token_t *token);

token_t *token_make_simple(token_kind_t token_kind);
token_t *token_make_str_lit(const char *str_lit);
token_t *token_make_char_lit(char char_lit);
token_t *token_make_int_lit(int int_lit);
token_t *token_make_name(const char *name);
