#ifndef KEBAB_CONSTRUCTOR_HPP
#define KEBAB_CONSTRUCTOR_HPP

#include <memory>
#include <vector>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab::Parser {

class Constructor : public AstNode {
protected:
  Constructor() = default;

  virtual void parse_type(Lexer &lexer) = 0;
  virtual void parse_body(Lexer &lexer) = 0;

public:
  std::string name;

  ~Constructor() override = default;

  static std::unique_ptr<Constructor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override = 0;
  // This pointer can be shared between instances of subclasses of the Constructor class and the
  // caller of this function (likely during compilation)
  virtual std::shared_ptr<Type> get_type() const = 0;
};

class ListConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer) final;
  void parse_body(Lexer &lexer) final;

public:
  std::shared_ptr<ListType> type;
  std::vector<std::unique_ptr<Statement>> body;

  static std::unique_ptr<ListConstructor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  std::shared_ptr<Type> get_type() const final { return this->type; }
};

class FunctionParameter : public AstNode {
public:
  std::string name;
  // This type is shared with the type of the function the parameter belongs to (FunctionType obj)
  // TODO: const & to maybe not need shared
  std::shared_ptr<Type> type;

  static std::unique_ptr<FunctionParameter> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class FunctionConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer) final;
  void parse_body(Lexer &lexer) final;

public:
  std::vector<std::unique_ptr<FunctionParameter>> parameters;
  std::shared_ptr<FunctionType> type;
  std::unique_ptr<Constructor> body;

  static std::unique_ptr<FunctionConstructor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  std::shared_ptr<Type> get_type() const final { return this->type; }
};

class PrimitiveConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer) final;
  void parse_body(Lexer &lexer) final;

public:
  std::shared_ptr<PrimitiveType> type;
  std::vector<std::unique_ptr<Statement>> body;

  static std::unique_ptr<PrimitiveConstructor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  std::shared_ptr<Type> get_type() const final { return this->type; }
};

} // namespace Kebab::Parser

#endif
