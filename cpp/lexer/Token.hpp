#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

enum TokenKind {
  // Keywords
  TOKEN_DEF,  // def
  TOKEN_SET,  // set
  TOKEN_MUT,  // mut
  TOKEN_NIL,  // nil
  TOKEN_IF,   // if
  TOKEN_ELIF, // elif
  TOKEN_ELSE, // else
  TOKEN_FN,   // fn

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

  // Values
  TOKEN_INT_LITERAL,    // 123
  TOKEN_FLOAT_LITERAL,  // 1.23
  TOKEN_CHAR_LITERAL,   // 'c'
  TOKEN_STRING_LITERAL, // "string"
  TOKEN_NAME,           // name

  // Special
  TOKEN_EOF,     // <end of file>
  TOKEN_ILLEGAL, // Illegal token, e.g. # @ % $, etc.
};

class Token {
public:
  TokenKind kind;
  std::variant<uint8_t, int64_t, float_t, std::string> value;

  Token(TokenKind kind) : kind(kind) {}
  Token(TokenKind kind, std::string word) : kind(kind), value(word) {
    // This constructor should only be called when kind is one of these
    assert(kind == TokenKind::TOKEN_NAME || kind == TokenKind::TOKEN_STRING_LITERAL);
  }
  Token(int64_t i) : kind(TokenKind::TOKEN_INT_LITERAL), value(i) {}
  Token(float_t f) : kind(TokenKind::TOKEN_FLOAT_LITERAL), value(f) {}
  Token(char c) : kind(TokenKind::TOKEN_CHAR_LITERAL), value(c) {}

  std::string to_string() const;
};

// class Token {
// public:
//   virtual std::string to_string() const = 0;
// };

#endif
