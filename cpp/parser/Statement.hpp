#ifndef KEBAB_STATEMENT_HPP
#define KEBAB_STATEMENT_HPP

#include "Parser.hpp"
#include "parser/Expression.hpp"

namespace Kebab {
namespace Parser {

class Constructor; // Cant include Constructor.hpp because of recursive includes

class Statement : public AstNode {
public:
  virtual ~Statement() = default;

  static std::unique_ptr<Statement> parse(Lexer &lexer);
};

class DefinitionStatement : public Statement {
public:
  bool is_mutable;
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<DefinitionStatement> parse(Lexer &lexer);
};

class AssignmentStatement : public Statement {
public:
  std::string name;
  std::unique_ptr<Constructor> constructor;

  static std::unique_ptr<AssignmentStatement> parse(Lexer &lexer);
};

class ExpressionStatement : public Statement {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<ExpressionStatement> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
