#ifndef KEBAB_COMPARISON_HPP
#define KEBAB_COMPARISON_HPP

#include <vector>

#include "Term.hpp"
#include "lexer/Lexer.hpp"
#include "lexer/Token.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class ComparisonOperator : AstNode {
public:
  enum Type {
    LT,  // <
    LE,  // <=
    EQ,  // ==
    NEQ, // ~=
    GT,  // >
    GE,  // >=
  } type;

  static constexpr bool is_comparison_operator(Token::Kind kind) {
    switch (kind) {
    case Token::LT:
    case Token::LE:
    case Token::EQ:
    case Token::NEQ:
    case Token::GT:
    case Token::GE:
      return true;

    default:
      return false;
    }
  }
  static std::unique_ptr<ComparisonOperator> parse(Lexer &lexer);
};

class Comparison : AstNode {
public:
  std::vector<std::unique_ptr<ComparisonOperator>> operators;
  std::vector<std::unique_ptr<Term>> terms;

  static std::unique_ptr<Comparison> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
