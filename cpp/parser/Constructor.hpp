#ifndef KEBAB_CONSTRUCTOR_HPP
#define KEBAB_CONSTRUCTOR_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

class Constructor : public AstNode {
protected:
  Constructor() = default;

  virtual void parse_type(Lexer &lexer) = 0;
  virtual void parse_body(Lexer &lexer) = 0;

public:
  std::vector<Statement *> body;

  static Constructor *parse(Lexer &lexer);
};

class ListConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  ListType *type;

  static ListConstructor *parse(Lexer &lexer);
};

class FunctionParameter : public AstNode {
public:
  std::string name;
  Type *type;

  static FunctionParameter *parse(Lexer &lexer);
};

class FunctionConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  std::vector<FunctionParameter *> parameters;
  FunctionType *type;

  static FunctionConstructor *parse(Lexer &lexer);
};

class PrimitiveConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  PrimitiveType *type;

  static PrimitiveConstructor *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
