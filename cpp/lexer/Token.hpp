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
  Token(const std::string &word) {
    if (word.compare("def") == 0)
      this->kind = DEF;
    else if (word.compare("set") == 0)
      this->kind = SET;
    else if (word.compare("mut") == 0)
      this->kind = MUT;
    else if (word.compare("if") == 0)
      this->kind = IF;
    else if (word.compare("elif") == 0)
      this->kind = ELIF;
    else if (word.compare("else") == 0)
      this->kind = ELSE;
    else if (word.compare("fn") == 0)
      this->kind = FN;
    else if (word.compare("true") == 0)
      this->kind = TRUE;
    else if (word.compare("false") == 0)
      this->kind = FALSE;
    else {
      this->kind = NAME;
      this->value = std::move(word);
    }
  };
  Token(Kind kind) : kind(kind) {}
  Token(Kind kind, const std::string &string) : kind(kind), value(string) {
    // This constructor should only be called when kind is one of these
    assert(kind == Kind::NAME || kind == Kind::STRING_LITERAL);
  }
  Token(int64_t i) : kind(Kind::INT_LITERAL), value(i) {}
  Token(float_t f) : kind(Kind::FLOAT_LITERAL), value(f) {}
  Token(char c) : kind(Kind::CHAR_LITERAL), value(c) {}

  // static Token from_string(const std::string &string);
  std::string to_string() const;
};

#endif
