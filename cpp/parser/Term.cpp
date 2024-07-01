#include <cassert>

#include "Term.hpp"
#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<TermOperator> TermOperator::parse(Lexer &lexer) {
  start_parsing("term-operator");
  std::unique_ptr<TermOperator> operator_ = std::make_unique<TermOperator>();

  switch (lexer.cur_token.type) {
  case Token::Type::PLUS:
    operator_->type = TermOperator::Type::PLUS;
    break;

  case Token::Type::MINUS:
    operator_->type = TermOperator::Type::MINUS;
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  lexer.advance();

  end_parsing("term-operator");
  return operator_;
}

void TermOperator::compile(Compiler &compiler) {}

std::unique_ptr<Term> Term::parse(Lexer &lexer) {
  start_parsing("term");
  std::unique_ptr<Term> term = std::make_unique<Term>();

  while (true) {
    term->factors.push_back(Factor::parse(lexer));

    if (TermOperator::is_term_operator(lexer.cur_token.type))
      term->operators.push_back(TermOperator::parse(lexer));
    else
      break;
  }

  end_parsing("term");
  return term;
}

void Term::compile(Compiler &compiler) {}

} // namespace Parser
} // namespace Kebab
