#ifndef KEBAB_COMPARISON_HPP
#define KEBAB_COMPARISON_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "parser/AstNode.hpp"
#include "parser/Term.hpp"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

class ComparisonOperator : public AstNode {
private:
  std::string to_string() const {
    switch (this->type) {
    case Type::LT:
      return "<";
    case Type::LE:
      return "<=";
    case Type::EQ:
      return "==";
    case Type::NEQ:
      return "~=";
    case Type::GT:
      return ">";
    case Type::GE:
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
  } type;
  llvm::Value *lhs;
  llvm::Value *rhs;

  static constexpr bool is_comparison_operator(Token::Type type) {
    switch (type) {
    case Token::Type::LT:
    case Token::Type::LE:
    case Token::Type::EQ:
    case Token::Type::NEQ:
    case Token::Type::GT:
    case Token::Type::GE:
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

} // namespace Parser
} // namespace Kebab

#endif
