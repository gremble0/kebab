#ifndef CONSTRUCTOR_HPP
#define CONSTRUCTOR_HPP

#include "parser/Parser.hpp"
#include <variant>

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
  static PrimitiveConstructor *parse(Lexer &lexer);
};

#endif
