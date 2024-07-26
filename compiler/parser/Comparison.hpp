#ifndef KEBAB_COMPARISON_HPP
#define KEBAB_COMPARISON_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "parser/AstNode.hpp"
#include "parser/Term.hpp"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

class ComparisonOperator : public AstNode {
private:
  std::string to_string() const {
    switch (this->type) {
      using enum Type;
    case LT:
      return "<";
    case LE:
      return "<=";
    case EQ:
      return "==";
    case NEQ:
      return "~=";
    case GT:
      return ">";
    case GE:
      return ">=";
    }
  }

public:
  enum class Type {
    LT,  // <
    LE,  // <=
    EQ,  // ==
    NEQ, // ~=
    GT,  // >
    GE,  // >=
  };
  Type type;
  llvm::Value *lhs;
  llvm::Value *rhs;

  static constexpr bool is_comparison_operator(Token::Type type) {
    switch (type) {
      using enum Token::Type;
    case LT:
    case LE:
    case EQ:
    case NEQ:
    case GT:
    case GE:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<ComparisonOperator> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class Comparison : public AstNode {
public:
  std::vector<std::unique_ptr<ComparisonOperator>> operators;
  std::vector<std::unique_ptr<Term>> terms;

  static std::unique_ptr<Comparison> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

} // namespace Kebab::Parser

#endif
