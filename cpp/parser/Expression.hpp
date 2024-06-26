#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <cstdint>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class CondExpression;
class NormalExpression;
class ConstructorExpression;

class Expression : public AstNode {
public:
  enum class Type : uint8_t {
    COND,
    NORMAL,
    FUNCTION,
  };
  Type type;

  static Expression *parse(Lexer &lexer);
};

class CondExpression : public Expression {
public:
  static CondExpression *parse(Lexer &lexer);
};

class NormalExpression : public Expression {
public:
  static NormalExpression *parse(Lexer &lexer);
};

class FunctionExpression : public Expression {
public:
  static FunctionExpression *parse(Lexer &lexer);
};

#endif
