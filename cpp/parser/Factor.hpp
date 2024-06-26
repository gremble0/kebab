#ifndef FACTOR_HPP
#define FACTOR_HPP

#include <vector>

#include "Primary.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

// TODO: Operator interface - is_operator(kind)
class FactorOperator : AstNode /*, Operator */ {
public:
  static constexpr bool is_factor_operator(Token::Kind kind) {
    switch (kind) {
    case Token::DIV:
    case Token::MULT:
      return true;

    default:
      return false;
    }
  }
};

class FactorPrefix : AstNode {
public:
  static constexpr bool is_factor_prefix(Token::Kind kind) {
    switch (kind) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }
};

class Factor : AstNode {
public:
  std::vector<FactorPrefix *> prefixes;
  std::vector<Primary *> primaries;
  std::vector<FactorOperator *> operators;

  static Factor *parse(Lexer &lexer);
};

#endif
