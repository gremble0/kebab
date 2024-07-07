#include <cassert>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/Term.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<TermOperator> TermOperator::parse(Lexer &lexer) {
  Logger::log_with_indent("term-operator");
  std::unique_ptr<TermOperator> operator_ = std::make_unique<TermOperator>();

  switch (lexer.cur_token->type) {
  case Token::Type::PLUS:
    operator_->type = TermOperator::Type::PLUS;
    break;

  case Token::Type::MINUS:
    operator_->type = TermOperator::Type::MINUS;
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token->to_string());
  }

  lexer.advance();

  Logger::log_with_dedent("term-operator");
  return operator_;
}

llvm::Value *TermOperator::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function TermOperator::compile");
}

std::unique_ptr<Term> Term::parse(Lexer &lexer) {
  Logger::log_with_indent("term");
  std::unique_ptr<Term> term = std::make_unique<Term>();

  while (true) {
    term->factors.push_back(Factor::parse(lexer));

    if (TermOperator::is_term_operator(lexer.cur_token->type))
      term->operators.push_back(TermOperator::parse(lexer));
    else
      break;
  }

  Logger::log_with_dedent("term");
  return term;
}

llvm::Value *Term::compile(Compiler &compiler) const {
  // TODO: some operator logic (this->operators)
  // for (std::unique_ptr<Factor> const &factor : this->factors)
  return this->factors.at(0)->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
