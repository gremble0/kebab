#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

class Token {
public:
  enum Kind {
    // Keywords
    DEF,  // def
    SET,  // set
    MUT,  // mut
    NIL,  // nil
    IF,   // if
    ELIF, // elif
    ELSE, // else
    FN,   // fn

    // Booleans
    TRUE,  // true
    FALSE, // false

    // Syntax
    COLON,      // :
    EQUALS,     // =
    COMMA,      // ,
    LPAREN,     // (
    RPAREN,     // )
    LBRACKET,   // [
    RBRACKET,   // ]
    FAT_RARROW, // =>

    // Operators
    PLUS,  // +
    MINUS, // -
    MULT,  // *
    DIV,   // /
    NOT,   // ~

    // Comparisons
    LT,  // <
    LE,  // <=
    EQ,  // ==
    NEQ, // ~=
    GT,  // >
    GE,  // >=

    // Values
    INT_LITERAL,    // 123
    FLOAT_LITERAL,  // 1.23
    CHAR_LITERAL,   // 'c'
    STRING_LITERAL, // "string"
    NAME,           // name

    // Special
    END_OF_FILE, // <end of file>
    ILLEGAL,     // Illegal token, e.g. # @ % $, etc.
  };

  Kind kind;
  std::variant<uint8_t, int64_t, float_t, std::string> value;

  Token() : kind(Kind::ILLEGAL) {}
  Token(Kind kind) : kind(kind) {}
  Token(const std::string &word);
  Token(Kind kind, const std::string &string) : kind(kind), value(string) {
    assert(kind == Kind::NAME || kind == Kind::STRING_LITERAL);
  }

  // Token(Kind kind, std::variant<uint8_t, int64_t, float_t, std::string> value)
  //     : kind(kind), value(value) {}

  Token(int64_t i) : kind(Kind::INT_LITERAL), value(i) {}
  Token(float_t f) : kind(Kind::FLOAT_LITERAL), value(f) {}
  Token(char c) : kind(Kind::CHAR_LITERAL), value(c) {}

  std::string to_string() const;
};

#endif
