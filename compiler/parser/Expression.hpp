#ifndef KEBAB_EXPRESSION_HPP
#define KEBAB_EXPRESSION_HPP

#include <vector>

#include "AndTest.hpp"
#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "llvm/IR/BasicBlock.h"

namespace Kebab {
namespace Parser {

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
  llvm::BasicBlock *merge_branch;

  void parse_test_body(Lexer &lexer);

  void parse_if(Lexer &lexer);
  void parse_elif(Lexer &lexer);
  void parse_elifs(Lexer &lexer);
  void parse_else(Lexer &lexer);

  llvm::Value *compile_branch(Compiler &compiler, size_t index) const;

public:
  std::vector<std::unique_ptr<Expression>> tests;
  std::vector<std::vector<std::unique_ptr<Statement>>> bodies;

  static std::unique_ptr<CondExpression> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};
// TODO: CondBody or something (maybe CondTest)

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

} // namespace Parser
} // namespace Kebab

#endif