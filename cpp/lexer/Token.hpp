#include <string>

enum class TokenKind {
  COLON,
  EQ,
};

class Token {
private:
  TokenKind kind;

public:
  Token(TokenKind kind);
  std::string to_string() const;
};
