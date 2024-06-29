#ifndef KEBAB_TYPE_HPP
#define KEBAB_TYPE_HPP

#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class Type : public AstNode {
protected:
  Type() = default;

public:
  static std::unique_ptr<Type> parse(Lexer &lexer);
};

class ListType : public Type {
public:
  std::unique_ptr<Type> content_type;

  static std::unique_ptr<ListType> parse(Lexer &lexer);
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
};

class PrimitiveType : public Type {
public:
  std::string name;

  static std::unique_ptr<PrimitiveType> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
