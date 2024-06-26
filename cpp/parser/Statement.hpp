#ifndef KEBAB_STATEMENT_HPP
#define KEBAB_STATEMENT_HPP

#include "Parser.hpp"
#include "parser/Expression.hpp"

class AssignmentStatement;
class DefinitionStatement;
class ExpressionStatement;
class Constructor; // Cant include Constructor.hpp because of recursive includes

class Statement : public AstNode {
public:
  enum Type {
    DEFINITION,
    ASSIGNMENT,
    EXPRESSION,
  };
  Type type;

  static Statement *parse(Lexer &lexer);
};

class DefinitionStatement : public Statement {
public:
  bool is_mutable;
  std::string name;
  Constructor *constructor;

  static DefinitionStatement *parse(Lexer &lexer);
};

class AssignmentStatement : public Statement {
public:
  std::string name;
  Constructor *constructor;

  static AssignmentStatement *parse(Lexer &lexer);
};

class ExpressionStatement : public Statement {
public:
  Expression *expression;

  static ExpressionStatement *parse(Lexer &lexer);
};

#endif
