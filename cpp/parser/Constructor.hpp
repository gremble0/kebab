#ifndef KEBAB_CONSTRUCTOR_HPP
#define KEBAB_CONSTRUCTOR_HPP

#include <vector>

#include "parser/Parser.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

class Constructor : public AstNode {
protected:
  Constructor();

public:
  std::vector<Statement *> body;

  static Constructor *parse(Lexer &lexer);
};

class ListConstructor : public Constructor {
public:
  ListType *type;

  static ListConstructor *parse(Lexer &lexer);
};

class FunctionConstructor : public Constructor {
public:
  FunctionType *type;

  static FunctionConstructor *parse(Lexer &lexer);
};

class PrimitiveConstructor : public Constructor {
public:
  static PrimitiveConstructor *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
