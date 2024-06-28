#ifndef KEBAB_CONSTRUCTOR_HPP
#define KEBAB_CONSTRUCTOR_HPP

#include <variant>
#include <vector>

#include "parser/Parser.hpp"
#include "parser/Statement.hpp"

namespace Kebab {
namespace Parser {

class ListConstructor;
class FunctionConstructor;
class PrimitiveConstructor;

class Constructor : public AstNode {
public:
  enum Type {
    LIST,
    FUNCTION,
    PRIMITIVE,
  };

  Type type;
  std::variant<ListConstructor *, FunctionConstructor *, PrimitiveConstructor *> value;

  static Constructor *parse(Lexer &lexer);
};

class ListConstructor : public Constructor {
public:
  static ListConstructor *parse(Lexer &lexer);
};

class FunctionConstructor : public Constructor {
public:
  static FunctionConstructor *parse(Lexer &lexer);
};

class PrimitiveConstructor : public Constructor {
public:
  std::vector<Statement *> body;

  static PrimitiveConstructor *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
