#ifndef COMPARISON_HPP
#define COMPARISON_HPP

#include "parser/Parser.hpp"
#include <vector>

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
  std::vector<ComparisonOperator> operators;
  std::vector<Term *> terms;
};

#endif
