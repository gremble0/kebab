#ifndef KEBAB_EXPRESSION_HPP
#define KEBAB_EXPRESSION_HPP

#include <vector>

#include "AndTest.hpp"
#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"

namespace Kebab::Parser {

class Statement; // cannot #include due to recursive inclusion
class FunctionConstructor;

class Expression : public AstNode {
public:
  virtual ~Expression() = default;

  static std::unique_ptr<Expression> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const override = 0;
};

class CondExpression : public Expression {
private:
  void parse_test_body(Lexer &lexer);

  void parse_if(Lexer &lexer);
  void parse_elif(Lexer &lexer);
  void parse_elifs(Lexer &lexer);
  void parse_else(Lexer &lexer);

public:
  std::vector<std::unique_ptr<Expression>> tests;
  std::vector<std::vector<std::unique_ptr<Statement>>> bodies;

  static std::unique_ptr<CondExpression> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class NormalExpression : public Expression {
public:
  std::vector<std::unique_ptr<AndTest>> and_tests;

  static std::unique_ptr<NormalExpression> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class FunctionExpression : public Expression {
public:
  std::unique_ptr<FunctionConstructor> function;

  static std::unique_ptr<FunctionExpression> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

} // namespace Kebab::Parser

#endif
