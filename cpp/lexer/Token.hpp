#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>

enum class TokenKind {
  // Keywords
  TOKEN_DEF,  // def
  TOKEN_SET,  // set
  TOKEN_MUT,  // mut
  TOKEN_NIL,  // nil
  TOKEN_IF,   // if
  TOKEN_ELIF, // elif
  TOKEN_ELSE, // else
  TOKEEN_FN,  // fn

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
  TOKEN_EOF, // <end of file>
};

class Token {
private:
  TokenKind kind;

  union {
    uint8_t c;     // some char value
    int64_t i;     // some int value
    float_t f;     // some float value
    std::string s; // either name or string value (depending on this->kind)
  };

public:
  Token(TokenKind kind) : kind(kind) {}
  Token(TokenKind kind, std::string str) : kind(kind), s(str) {
    // This constructor should only be called when kind is one of these
    assert(kind == TokenKind::TOKEN_NAME || kind == TokenKind::TOKEN_STRING_LITERAL);
  }
  Token(int64_t i) : kind(TokenKind::TOKEN_INT_LITERAL), i(i) {}
  Token(float_t f) : kind(TokenKind::TOKEN_FLOAT_LITERAL), f(f) {}
  ~Token();

  std::string to_string() const;
};
