#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>

#include "AndTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class CondExpression;
class NormalExpression;
class ConstructorExpression;

class Expression : public AstNode {
public:
  static Expression *parse(Lexer &lexer);
};

class CondExpression : public Expression {
public:
  static CondExpression *parse(Lexer &lexer);
};

class NormalExpression : public Expression {
public:
  std::vector<AndTest *> andTests;

  static NormalExpression *parse(Lexer &lexer);
};

class FunctionExpression : public Expression {
public:
  static FunctionExpression *parse(Lexer &lexer);
};

#endif
