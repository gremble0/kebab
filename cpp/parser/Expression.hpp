#ifndef KEBAB_EXPRESSION_HPP
#define KEBAB_EXPRESSION_HPP

#include <vector>

#include "AndTest.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class CondExpression;
class NormalExpression;
class ConstructorExpression;

class Expression : public AstNode {
public:
  static std::unique_ptr<Expression> parse(Lexer &lexer);
};

class CondExpression : public Expression {
public:
  static std::unique_ptr<CondExpression> parse(Lexer &lexer);
};

class NormalExpression : public Expression {
public:
  std::vector<std::unique_ptr<AndTest>> and_tests;

  static std::unique_ptr<NormalExpression> parse(Lexer &lexer);
};

class FunctionExpression : public Expression {
public:
  static std::unique_ptr<FunctionExpression> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
