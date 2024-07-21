#pragma once

#include "nonstdlib/nstring.h"
#include "utils/utils.h"
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
  TOKEN_LBRACKET,   // [
  TOKEN_RBRACKET,   // ]
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
  TOKEN_CHAR_LITERAL,   // c
  TOKEN_STRING_LITERAL, // string
  TOKEN_INT_LITERAL,    // 123
  // TODO: TOKEN_FLOAT_LITERAL,
  TOKEN_NAME,
} token_kind_t;

// Keywords
static const string_t token_string_def = string_of("def");
static const string_t token_string_set = string_of("set");
static const string_t token_string_mut = string_of("mut");
static const string_t token_string_if = string_of("if");
static const string_t token_string_elif = string_of("elif");
static const string_t token_string_else = string_of("else");
static const string_t token_string_nil = string_of("nil");
// Constructors
static const string_t token_string_char = string_of("char");
static const string_t token_string_string = string_of("string");
static const string_t token_string_int = string_of("int");
static const string_t token_string_bool = string_of("bool");
static const string_t token_string_fn = string_of("fn");
static const string_t token_string_list = string_of("list");
// Booleans
static const string_t token_string_true = string_of("true");
static const string_t token_string_false = string_of("false");
// Syntax
static const string_t token_string_colon = string_of(":");
static const string_t token_string_equals = string_of("=");
static const string_t token_string_comma = string_of(",");
static const string_t token_string_lparen = string_of("(");
static const string_t token_string_rparen = string_of(")");
static const string_t token_string_lbrace = string_of("[");
static const string_t token_string_rbrace = string_of("]");
static const string_t token_string_fat_rarrow = string_of("=>");
// Operators
static const string_t token_string_plus = string_of("+");
static const string_t token_string_minus = string_of("-");
static const string_t token_string_mult = string_of("*");
static const string_t token_string_div = string_of("/");
static const string_t token_string_not = string_of("~");
// Comparisons
static const string_t token_string_lt = string_of("<");
static const string_t token_string_le = string_of("<=");
static const string_t token_string_eq = string_of("==");
static const string_t token_string_neq = string_of("~=");
static const string_t token_string_gt = string_of(">");
static const string_t token_string_ge = string_of(">=");
// Special
static const string_t token_string_eof = string_of("<eof>");
// Values and literals (without any actual contents)
static const string_t token_string_char_lit = string_of("<char-lit>");
static const string_t token_string_string_lit = string_of("<string-lit>");
static const string_t token_string_int_lit = string_of("<int-lit>");
static const string_t token_string_name = string_of("<name>");

typedef struct {
  token_kind_t kind;
  union {
    string_t *name;
    string_t *string_literal;
    uint8_t char_literal;    // TODO: int for unicode or something? idk
    int64_t integer_literal; // Ints in kebab are 64 bits
    // Don't need to store bool_literal as the token_kind is sufficient
  };
  span_t span;
} token_t;

string_t *token_to_string(const token_t *token);
string_t *token_kind_to_string(token_kind_t kind);
void token_free(token_t *token);

token_t *token_make_simple(token_kind_t token_kind, span_t span);
token_t *token_make_name(string_t *name, span_t span);
token_t *token_make_str_lit(string_t *str_lit, span_t span);
token_t *token_make_char_lit(uint8_t char_lit, span_t span);
token_t *token_make_int_lit(int64_t int_lit, span_t span);
