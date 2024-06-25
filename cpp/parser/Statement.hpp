#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Parser.hpp"

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
  static DefinitionStatement *parse(Lexer &lexer);
};

class AssignmentStatement : public Statement {
public:
  static AssignmentStatement *parse(Lexer &lexer);
};

class ExpressionStatement : public Statement {
public:
  static ExpressionStatement *parse(Lexer &lexer);
};

#endif
