#ifndef KEBAB_FACTOR_HPP
#define KEBAB_FACTOR_HPP

#include <vector>

#include "Primary.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

// TODO: Operator interface - is_operator(kind)
class FactorOperator : AstNode /*, Operator */ {
public:
  enum Type {
    MULT,
    DIV,
  } type;

  static constexpr bool is_factor_operator(Token::Kind kind) {
    switch (kind) {
    case Token::DIV:
    case Token::MULT:
      return true;

    default:
      return false;
    }
  }

  static FactorOperator *parse(Lexer &lexer);
};

class FactorPrefix : AstNode {
public:
  enum Type {
    PLUS,
    MINUS,
  } type;

  static constexpr bool is_factor_prefix(Token::Kind kind) {
    switch (kind) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }

  static FactorPrefix *parse(Lexer &lexer);
};

class Factor : AstNode {
public:
  std::vector<FactorPrefix *> prefixes;
  std::vector<Primary *> primaries;
  std::vector<FactorOperator *> operators;

  static Factor *parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
