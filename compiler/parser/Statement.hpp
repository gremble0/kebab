#ifndef KEBAB_STATEMENT_HPP
#define KEBAB_STATEMENT_HPP

#include <optional>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Expression.hpp"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

class Constructor; // Cant include Constructor.hpp because of recursive includes

class Statement : public AstNode {
public:
  ~Statement() override = default;

  static std::unique_ptr<Statement> parse(Lexer &lexer);
  static std::optional<std::unique_ptr<Statement>> try_parse_statement(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override = 0;
  virtual bool is_expression() const = 0;
};

class DefinitionStatement : public Statement {
public:
  bool is_mutable;
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<DefinitionStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  bool is_expression() const final { return false; }
};

class AssignmentStatement : public Statement {
public:
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<AssignmentStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  bool is_expression() const final { return false; }
};

class ExpressionStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<ExpressionStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
  bool is_expression() const final { return true; }
};

} // namespace Kebab::Parser

#endif
