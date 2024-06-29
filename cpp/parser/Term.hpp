#ifndef KEBAB_TERM_HPP
#define KEBAB_TERM_HPP

#include <vector>

#include "Factor.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

// this is a prefix?? dont remember will look into and maybe rename
class TermOperator : AstNode {
public:
  enum Type {
    PLUS,
    MINUS,
  } type;

  static constexpr bool is_term_operator(Token::Kind kind) {
    switch (kind) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<TermOperator> parse(Lexer &lexer);
};

class Term : AstNode {
public:
  std::vector<std::unique_ptr<Factor>> factors;
  std::vector<std::unique_ptr<TermOperator>> operators;

  static std::unique_ptr<Term> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
