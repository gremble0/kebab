#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <variant>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class CondExpression;
class NormalExpression;
class ConstructorExpression;

class Expression : AstNode {
public:
  enum Type {
    COND,
    NORMAL,
    CONSTRUCTOR,
  };

  Type type;
  std::variant<CondExpression *, NormalExpression *, ConstructorExpression *> value;

  static Expression *parse(Lexer &lexer);
};

class CondExpression : Expression {};

class NormalExpression : Expression {};

class ConstructorExpression : Expression {};

#endif
