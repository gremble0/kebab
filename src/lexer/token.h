#pragma once

#include "nonstdlib/nstring.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
  // Keywords (has to be at top of enum for array for iterations)
  TOKEN_DEF, // def
  TOKEN_SET, // set
  TOKEN_MUT, // mut
  // TOKEN_DEC ? declare variable with type?
  TOKEN_NIL,  // nil
  TOKEN_IF,   // if
  TOKEN_ELIF, // elif
  TOKEN_ELSE, // else

  // Constructors
  TOKEN_CHAR,
  TOKEN_STRING,
  TOKEN_INT,
  TOKEN_BOOL,
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
  const string_t *word;
  token_kind_t kind;
} reserved_word_map_t;

static const reserved_word_map_t reserved_word_map[] = {
    // Keywords
    {&(string_t){.s = "def", .len = sizeof("def")}, TOKEN_DEF},
    {&(string_t){.s = "set", .len = sizeof("set")}, TOKEN_SET},
    {&(string_t){.s = "mut", .len = sizeof("mut")}, TOKEN_MUT},
    {&(string_t){.s = "if", .len = sizeof("if")}, TOKEN_IF},
    {&(string_t){.s = "elif", .len = sizeof("elif")}, TOKEN_ELIF},
    {&(string_t){.s = "else", .len = sizeof("else")}, TOKEN_ELSE},

    {&(string_t){.s = "nil", .len = sizeof("nil")}, TOKEN_NIL},

    // Constructors
    {&(string_t){.s = "char", .len = sizeof("char")}, TOKEN_CHAR},
    {&(string_t){.s = "string", .len = sizeof("string")}, TOKEN_STRING},
    {&(string_t){.s = "int", .len = sizeof("int")}, TOKEN_INT},
    {&(string_t){.s = "bool", .len = sizeof("bool")}, TOKEN_BOOL},
    {&(string_t){.s = "fn", .len = sizeof("fn")}, TOKEN_FN},
    {&(string_t){.s = "list", .len = sizeof("list")}, TOKEN_LIST},

    // Booleans
    {&(string_t){.s = "true", .len = sizeof("true")}, TOKEN_TRUE},
    {&(string_t){.s = "false", .len = sizeof("false")}, TOKEN_FALSE},
};

static const string_t *token_kind_map[] = {
    // Keywords
    [TOKEN_DEF] = &(string_t){.s = "def", .len = sizeof("def")},
    [TOKEN_SET] = &(string_t){.s = "set", .len = sizeof("set")},
    [TOKEN_MUT] = &(string_t){.s = "mut", .len = sizeof("mut")},
    [TOKEN_IF] = &(string_t){.s = "if", .len = sizeof("if")},
    [TOKEN_ELIF] = &(string_t){.s = "elif", .len = sizeof("elif")},
    [TOKEN_ELSE] = &(string_t){.s = "else", .len = sizeof("else")},
    [TOKEN_NIL] = &(string_t){.s = "nil", .len = sizeof("nil")},

    // Constructors
    [TOKEN_CHAR] = &(string_t){.s = "char", .len = sizeof("char")},
    [TOKEN_STRING] = &(string_t){.s = "string", .len = sizeof("string")},
    [TOKEN_INT] = &(string_t){.s = "int", .len = sizeof("int")},
    [TOKEN_BOOL] = &(string_t){.s = "bool", .len = sizeof("bool")},
    [TOKEN_FN] = &(string_t){.s = "fn", .len = sizeof("fn")},
    [TOKEN_LIST] = &(string_t){.s = "list", .len = sizeof("list")},

    // Booleans
    [TOKEN_TRUE] = &(string_t){.s = "true", .len = sizeof("true")},
    [TOKEN_FALSE] = &(string_t){.s = "false", .len = sizeof("false")},

    // Syntax
    [TOKEN_COLON] = &(string_t){.s = ":", .len = sizeof(":")},
    [TOKEN_EQUALS] = &(string_t){.s = "=", .len = sizeof("=")},
    [TOKEN_COMMA] = &(string_t){.s = ",", .len = sizeof(",")},
    [TOKEN_LPAREN] = &(string_t){.s = "(", .len = sizeof("(")},
    [TOKEN_RPAREN] = &(string_t){.s = ")", .len = sizeof(")")},
    [TOKEN_LBRACE] = &(string_t){.s = "[", .len = sizeof("[")},
    [TOKEN_RBRACE] = &(string_t){.s = "]", .len = sizeof("]")},
    [TOKEN_FAT_RARROW] = &(string_t){.s = "=>", .len = sizeof("=>")},

    // Operators
    [TOKEN_PLUS] = &(string_t){.s = "+", .len = sizeof("+")},
    [TOKEN_MINUS] = &(string_t){.s = "-", .len = sizeof("-")},
    [TOKEN_MULT] = &(string_t){.s = "*", .len = sizeof("*")},
    [TOKEN_DIV] = &(string_t){.s = "/", .len = sizeof("/")},

    // Comparisons
    [TOKEN_NOT] = &(string_t){.s = "~", .len = sizeof("~")},
    [TOKEN_LT] = &(string_t){.s = "<", .len = sizeof("<")},
    [TOKEN_LE] = &(string_t){.s = "<=", .len = sizeof("<=")},
    [TOKEN_EQ] = &(string_t){.s = "==", .len = sizeof("==")},
    [TOKEN_NEQ] = &(string_t){.s = "~=", .len = sizeof("~=")},
    [TOKEN_GT] = &(string_t){.s = ">", .len = sizeof(">")},
    [TOKEN_GE] = &(string_t){.s = ">=", .len = sizeof(">=")},

    // Special
    [TOKEN_EOF] = &(string_t){.s = "<eof>", .len = sizeof("<eof>")},

    // Values
    [TOKEN_CHAR_LITERAL] =
        &(string_t){.s = "<char-literal>", .len = sizeof("<char-literal>")},
    [TOKEN_STRING_LITERAL] =
        &(string_t){.s = "<string-literal>", .len = sizeof("<string-literal>")},
    [TOKEN_INTEGER_LITERAL] = &(string_t){.s = "<integer-literal>",
                                          .len = sizeof("<integer-literal>")},
    [TOKEN_NAME] = &(string_t){.s = "<name>", .len = sizeof("<name>")},
};

typedef struct {
  token_kind_t kind;
  union {
    string_t *name;
    string_t *string_literal;
    uint8_t char_literal;    // TODO: int for unicode or something? idk
    int64_t integer_literal; // Ints in kebab are 64 bits
    // Don't need to store bool_literal as the token_kind is sufficient
  };
} token_t;

string_t *token_to_string(const token_t *token);
string_t *token_kind_to_string(token_kind_t kind);
void token_free(token_t *token);

token_t *token_make_simple(token_kind_t token_kind);
token_t *token_make_name(string_t *name);
token_t *token_make_str_lit(string_t *str_lit);
token_t *token_make_char_lit(uint8_t char_lit);
token_t *token_make_int_lit(int64_t int_lit);
