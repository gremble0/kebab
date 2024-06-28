#include <cassert>

#include "Term.hpp"
#include "lexer/Lexer.hpp"

namespace Parser {

TermOperator *TermOperator::parse(Lexer &lexer) {
  start_parsing("term-operator");
  TermOperator *operator_ = new TermOperator();

  switch (lexer.cur_token.kind) {
  case Token::Kind::PLUS:
    operator_->type = TermOperator::Type::PLUS;
    break;

  case Token::Kind::MINUS:
    operator_->type = TermOperator::Type::MINUS;
    break;

  default:
    // Unreachable
    assert(false);
  }

  end_parsing("term-operator");
  return operator_;
}

Term *Term::parse(Lexer &lexer) {
  start_parsing("term");
  Term *term = new Term();

  while (true) {
    term->factors.push_back(Factor::parse(lexer));

    if (TermOperator::is_term_operator(lexer.cur_token.kind))
      term->operators.push_back(TermOperator::parse(lexer));
    else
      break;
  }

  end_parsing("term");
  return term;
}
} // namespace Parser
