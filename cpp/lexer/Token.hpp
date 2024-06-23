#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>

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
  virtual ~Token() = default;
  virtual TokenKind kind() const = 0;
  virtual std::string to_string() const = 0;
};

class TokenSimple : public Token {
private:
  TokenKind kind_;

public:
  TokenSimple(TokenKind kind) : kind_(kind) {}
  TokenKind kind() const override { return this->kind_; }
  std::string to_string() const override;
};

class TokenIntLiteral : public Token {
private:
  int64_t literal;

public:
  TokenIntLiteral(int64_t literal) : literal(literal) {}
  TokenKind kind() const override { return TokenKind::TOKEN_INT_LITERAL; }
  std::string to_string() const override { return std::to_string(this->literal); }
};

class TokenFloatLiteral : public Token {
private:
  float_t literal;

public:
  TokenFloatLiteral(float_t literal) : literal(literal) {}
  TokenKind kind() const override { return TokenKind::TOKEN_FLOAT_LITERAL; }
  std::string to_string() const override { return std::to_string(this->literal); }
};

class TokenCharLiteral : public Token {
private:
  char literal;

public:
  TokenCharLiteral(char literal) : literal(literal) {}
  TokenKind kind() const override { return TokenKind::TOKEN_CHAR_LITERAL; }
  std::string to_string() const override { return std::to_string(this->literal); }
};

class TokenStringLiteral : public Token {
private:
  std::string literal;

public:
  TokenStringLiteral(std::string literal) : literal(literal) {}
  TokenKind kind() const override { return TokenKind::TOKEN_STRING_LITERAL; }
  std::string to_string() const override { return this->literal; }
};

class TokenName : public Token {
private:
  std::string name;

public:
  TokenName(std::string name) : name(name) {}
  TokenKind kind() const override { return TokenKind::TOKEN_NAME; }
  std::string to_string() const override { return this->name; }
};

#endif
