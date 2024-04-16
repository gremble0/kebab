#pragma once

#include <stddef.h>

typedef enum {
  // Keywords (has to be at top of enum for array for iterations)
  TOKEN_DEF, // def
  TOKEN_SET, // set
  TOKEN_MUT, // mut
  TOKEN_NIL, // nil
  // TOKEN_DEC ? declare variable with type?

  // Constructors
  TOKEN_FN,   // fn
  TOKEN_LIST, // list

  // Booleans
  TOKEN_TRUE,  // true
  TOKEN_FALSE, // false

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

  // Special
  TOKEN_EOF, // <end of file>

  // Values
  TOKEN_CHAR_LITERAL,    // c
  TOKEN_STRING_LITERAL,  // string
  TOKEN_INTEGER_LITERAL, // 123
  // TODO: TOKEN_FLOAT_LITERAL,
  TOKEN_NAME,
} token_kind_t;

typedef struct {
  const char *word;
  token_kind_t kind;
} reserved_word_map_t;

static const reserved_word_map_t reserved_word_map[] = {
    // Keywords
    {"def", TOKEN_DEF},
    {"mut", TOKEN_MUT},
    {"nil", TOKEN_NIL},
    {"set", TOKEN_SET},

    // Constructors
    {"fn", TOKEN_FN},
    {"list", TOKEN_LIST},

    // Booleans
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
};

static const char *token_kind_map[] = {
    // Keywords
    [TOKEN_DEF] = "def",
    [TOKEN_SET] = "set",
    [TOKEN_MUT] = "mut",
    [TOKEN_NIL] = "nil",

    // Constructors
    [TOKEN_FN] = "fn",
    [TOKEN_LIST] = "list",

    // Booleans
    [TOKEN_TRUE] = "true",
    [TOKEN_FALSE] = "false",

    // Syntax
    [TOKEN_COLON] = ":",
    [TOKEN_EQUALS] = "=",
    [TOKEN_COMMA] = ",",
    [TOKEN_LPAREN] = "(",
    [TOKEN_RPAREN] = ")",
    [TOKEN_LBRACE] = "[",
    [TOKEN_RBRACE] = "]",
    [TOKEN_FAT_RARROW] = "=>",

    // Operators
    [TOKEN_PLUS] = "+",
    [TOKEN_MINUS] = "-",
    [TOKEN_MULT] = "*",
    [TOKEN_DIV] = "/",

    // Comparisons
    [TOKEN_NOT] = "~",
    [TOKEN_LT] = "<",
    [TOKEN_LE] = "<=",
    [TOKEN_EQ] = "==",
    [TOKEN_NEQ] = "~=",
    [TOKEN_GT] = ">",
    [TOKEN_GE] = ">=",

    // Special
    [TOKEN_EOF] = "<eof>",

    // Values
    [TOKEN_CHAR_LITERAL] = "<char-literal>",
    [TOKEN_STRING_LITERAL] = "<string-literal>",
    [TOKEN_INTEGER_LITERAL] = "<integer-literal>",
    [TOKEN_NAME] = "<name>",
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
