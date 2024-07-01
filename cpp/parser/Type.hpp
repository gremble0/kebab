#ifndef KEBAB_TYPE_HPP
#define KEBAB_TYPE_HPP

#include <memory>
#include <vector>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class Type : public AstNode {
protected:
  Type() = default;

public:
  virtual ~Type() = default;

  static std::unique_ptr<Type> parse(Lexer &lexer);
  virtual void compile(Compiler &compiler) = 0;
};

class ListType : public Type {
public:
  std::unique_ptr<Type> content_type;

  static std::unique_ptr<ListType> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

class FunctionType : public Type {
private:
  void parse_parameter_types(Lexer &lexer);
  void parse_return_type(Lexer &lexer);

public:
  // The parameter types are shared with the types of each parameter in the function
  std::vector<std::shared_ptr<Type>> parameter_types;
  // This return type is shared with the type of the functions body
  std::shared_ptr<Type> return_type;

  static std::unique_ptr<FunctionType> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

class PrimitiveType : public Type {
public:
  std::string name;

  static std::unique_ptr<PrimitiveType> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

} // namespace Parser
} // namespace Kebab

#endif
