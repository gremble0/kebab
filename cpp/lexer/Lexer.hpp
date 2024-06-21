#include <string>
class Lexer {
private:
  std::string path;

public:
  Lexer(std::string path);
  void advance();
};
