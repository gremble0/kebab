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
public:
  enum Type {
    LT,  // <
    LE,  // <=
    EQ,  // ==
    NEQ, // ~=
    GT,  // >
    GE,  // >=
  } type;

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
  llvm::Value *compile(Compiler &compiler) const;
};

class Comparison : public AstNode {
public:
  std::vector<std::unique_ptr<ComparisonOperator>> operators;
  std::vector<std::unique_ptr<Term>> terms;

  static std::unique_ptr<Comparison> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
