#include <optional>
#include <string>

#include "Token.hpp"

class Lexer {
private:
  std::string path;
  std::optional<Token> cur_token;

public:
  Lexer(std::string path) : path(path) {}
  // void advance();
};
