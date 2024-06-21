#include "Token.hpp"

Token::Token(TokenKind kind) : kind(kind) {}

std::string Token::to_string() const {
  switch (this->kind) {
  case TokenKind::COLON:
    return ":";

  case TokenKind::EQ:
    return "=";
  }

  return "";
}
