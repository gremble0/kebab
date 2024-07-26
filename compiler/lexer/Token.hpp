#ifndef KEBAB_TOKEN_HPP
#define KEBAB_TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <variant>

#include "lexer/Span.hpp"

namespace Kebab {

class Token {
public:
  enum class Type {
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
  };
  const Type type;

  const std::variant<uint8_t, int64_t, double_t, std::string> value;
  const Span span;

  Token(Type type, const Span &span) : type(type), span(span) {}
  Token(Type type, const Span &span, std::variant<uint8_t, int64_t, double_t, std::string> value)
      : type(type), value(value), span(span) {}
  Token(const Span &span, const std::string &word)
      : type(Token::determine_type(word)), value(Token::determine_value(this->type, word)),
        span(span) {}

  std::string to_string_short() const;
  std::string to_string_verbose() const;
  static std::string type_to_string(Type type);

private:
  static constexpr Type determine_type(std::string_view word) {
    if (word == "def")
      return Type::DEF;
    else if (word == "set")
      return Type::SET;
    else if (word == "mut")
      return Type::MUT;
    else if (word == "if")
      return Type::IF;
    else if (word == "elif")
      return Type::ELIF;
    else if (word == "else")
      return Type::ELSE;
    else if (word == "fn")
      return Type::FN;
    else if (word == "list")
      return Type::LIST;
    else if (word == "true")
      return Type::TRUE;
    else if (word == "false")
      return Type::FALSE;
    else if (word == "and")
      return Type::AND;
    else if (word == "or")
      return Type::OR;
    else
      return Type::NAME;
  }

  static std::variant<uint8_t, int64_t, double_t, std::string>
  determine_value(Type type, const std::string &word) {
    if (type == Type::NAME)
      return word;
    else
      return {};
  }
};

} // namespace Kebab

#endif
