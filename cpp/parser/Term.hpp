#ifndef KEBAB_TERM_HPP
#define KEBAB_TERM_HPP

#include <vector>

#include "Factor.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class TermOperator {
public:
  static constexpr bool is_term_operator(Token::Kind kind) {
    switch (kind) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }
};

class Term : AstNode {
public:
  std::vector<Factor *> factors;
  std::vector<TermOperator *> operators;

  static Term *parse(Lexer &lexer);
};

#endif
