#ifndef KEBAB_TYPE_HPP
#define KEBAB_TYPE_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class Type : public AstNode {
protected:
  Type();

public:
  static Type *parse(Lexer &lexer);
};

class ListType : public Type {
public:
  Type *content_type;

  static ListType *parse(Lexer &lexer);
};

class FunctionType : public Type {
public:
  std::vector<Type *> parameter_types;
  Type *return_type;

  static FunctionType *parse(Lexer &lexer);
};

class PrimitiveType : public Type {
public:
  std::string name;

  static PrimitiveType *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
