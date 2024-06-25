#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

#include "Span.hpp"

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
  Span span;
  std::variant<uint8_t, int64_t, float_t, std::string> value;

  // make default constructors?
  Token() : kind(Kind::ILLEGAL), span(Position(0, 0), Position(0, 0)) {}
  Token(Kind kind, Span span) : kind(kind), span(span) {}
  Token(Kind kind, Span span, std::variant<uint8_t, int64_t, float_t, std::string> value)
      : kind(kind), span(span), value(value) {}
  Token(Span span, std::string word);

  std::string to_string() const;
};

#endif
