#ifndef KEBAB_TYPE_HPP
#define KEBAB_TYPE_HPP

#include <memory>
#include <vector>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"

namespace Kebab::Parser {

class Type : public AstNode {
protected:
  Type() = default;

public:
  ~Type() override = default;

  static std::unique_ptr<Type> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override = 0;
  virtual llvm::Type *get_llvm_type(Compiler &compiler) const = 0;
};

class ListType : public Type {
public:
  std::unique_ptr<Type> content_type;

  static std::unique_ptr<ListType> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  // ScalableVectorType may be right here? will look into
  llvm::ScalableVectorType *get_llvm_type(Compiler &compiler) const final;
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
  llvm::Value *compile(Compiler &compiler) const final;
  // FunctionType is surely the right type here?
  llvm::FunctionType *get_llvm_type(Compiler &compiler) const final;
};

class PrimitiveType : public Type {
public:
  std::string name;

  static std::unique_ptr<PrimitiveType> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  llvm::Type *get_llvm_type(Compiler &compiler) const final;
};

} // namespace Kebab::Parser

#endif
