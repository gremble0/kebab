#include <cassert>

#include "lexer/Lexer.hpp"
#include "parser/Term.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<TermOperator> TermOperator::parse(Lexer &lexer) {
  std::unique_ptr<TermOperator> operator_ = std::make_unique<TermOperator>();
  operator_->start_parsing(lexer, "<term-operator>");

  switch (lexer.cur_token->type) {
  case Token::Type::PLUS:
    operator_->type = TermOperator::Type::PLUS;
    break;

  case Token::Type::MINUS:
    operator_->type = TermOperator::Type::MINUS;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.cur_token->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</term-operator>");
  return operator_;
}

llvm::Value *TermOperator::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function TermOperator::compile");
}

std::unique_ptr<Term> Term::parse(Lexer &lexer) {
  std::unique_ptr<Term> term = std::make_unique<Term>();
  term->start_parsing(lexer, "<term>");

  while (true) {
    term->factors.push_back(Factor::parse(lexer));

    if (TermOperator::is_term_operator(lexer.cur_token->type))
      term->operators.push_back(TermOperator::parse(lexer));
    else
      break;
  }

  term->finish_parsing(lexer, "</term>");
  return term;
}

llvm::Value *Term::compile(Compiler &compiler) const {
  // TODO: some operator logic (this->operators)
  // for (std::unique_ptr<Factor> const &factor : this->factors)
  return this->factors.at(0)->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
