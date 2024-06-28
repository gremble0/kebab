#ifndef KEBAB_EXPRESSION_HPP
#define KEBAB_EXPRESSION_HPP

#include <vector>

#include "AndTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Parser {

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
  std::vector<AndTest *> and_tests;

  static NormalExpression *parse(Lexer &lexer);
};

class FunctionExpression : public Expression {
public:
  static FunctionExpression *parse(Lexer &lexer);
};

} // namespace Parser

#endif
