#ifndef KEBAB_COMPARISON_HPP
#define KEBAB_COMPARISON_HPP

#include <vector>

#include "Term.hpp"
#include "parser/Parser.hpp"

class ComparisonOperator {
public:
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
};

class Comparison : AstNode {
public:
  std::vector<ComparisonOperator *> operators;
  std::vector<Term *> terms;

  static Comparison *parse(Lexer &lexer);
};

#endif
