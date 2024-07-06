#ifndef KEBAB_STATEMENT_HPP
#define KEBAB_STATEMENT_HPP

#include <optional>

#include "lexer/Lexer.hpp"
#include "parser/Expression.hpp"
#include "parser/Parser.hpp"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

class Constructor; // Cant include Constructor.hpp because of recursive includes

class Statement : public AstNode {
public:
  virtual ~Statement() = default;

  static std::unique_ptr<Statement> parse(Lexer &lexer);
  static std::optional<std::unique_ptr<Statement>> try_parse_expression_statement(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const = 0;
};

class DefinitionStatement : public Statement {
public:
  bool is_mutable;
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<DefinitionStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const;
};

class AssignmentStatement : public Statement {
public:
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<AssignmentStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const;
};

class ExpressionStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<ExpressionStatement> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
