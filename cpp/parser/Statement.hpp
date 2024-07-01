#ifndef KEBAB_STATEMENT_HPP
#define KEBAB_STATEMENT_HPP

#include "Parser.hpp"
#include "compiler/Compiler.hpp"
#include "parser/Expression.hpp"

namespace Kebab {
namespace Parser {

class Constructor; // Cant include Constructor.hpp because of recursive includes

class Statement : public AstNode {
public:
  virtual ~Statement() = default;

  static std::unique_ptr<Statement> parse(Lexer &lexer);
  virtual void compile(Compiler &compiler) = 0;
};

class DefinitionStatement : public Statement {
public:
  bool is_mutable;
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<DefinitionStatement> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

class AssignmentStatement : public Statement {
public:
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<AssignmentStatement> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

class ExpressionStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<ExpressionStatement> parse(Lexer &lexer);
  void compile(Compiler &compiler);
};

} // namespace Parser
} // namespace Kebab

#endif
