#ifndef KEBAB_TOKEN_HPP
#define KEBAB_TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

#include "Span.hpp"

namespace Kebab {

class Token {
public:
  enum Type {
    // Keywords
    DEF,  // def
    SET,  // set
    MUT,  // mut
    NIL,  // nil
    IF,   // if
    ELIF, // elif
    ELSE, // else

    // Special constructors
    FN,   // fn
    LIST, // list

    // Booleans
    TRUE,  // true
    FALSE, // false

    // Boolean operators
    AND, // and
    OR,  // or

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
  } type;

  Span span;
  std::variant<uint8_t, int64_t, float_t, std::string> value;

  // make default constructors?
  Token(Type type, Span span) : type(type), span(span) {}
  Token(Type type, Span span, std::variant<uint8_t, int64_t, float_t, std::string> value)
      : type(type), span(span), value(value) {}
  Token(Span span, std::string word);

  std::string to_string() const;
  static std::string type_to_string(Type type);
};

} // namespace Kebab

#endif
