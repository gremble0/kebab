#include <cassert>

#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Term.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<ComparisonOperator> ComparisonOperator::parse(Lexer &lexer) {
  std::unique_ptr<ComparisonOperator> operator_ = std::make_unique<ComparisonOperator>();
  operator_->start_parsing(lexer, "<comparison-operator>");

  switch (lexer.cur_token->type) {
  case Token::Type::LT:
    operator_->type = ComparisonOperator::Type::LT;
    break;

  case Token::Type::LE:
    operator_->type = ComparisonOperator::Type::LE;
    break;

  case Token::Type::EQ:
    operator_->type = ComparisonOperator::Type::EQ;
    break;

  case Token::Type::NEQ:
    operator_->type = ComparisonOperator::Type::NEQ;
    break;

  case Token::Type::GT:
    operator_->type = ComparisonOperator::Type::GT;
    break;

  case Token::Type::GE:
    operator_->type = ComparisonOperator::Type::GE;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.cur_token->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</comparison-operator>");
  return operator_;
}

llvm::Value *ComparisonOperator::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function ComparisonOperator::compile");
}

std::unique_ptr<Comparison> Comparison::parse(Lexer &lexer) {
  std::unique_ptr<Comparison> comparison = std::make_unique<Comparison>();
  comparison->start_parsing(lexer, "<comparison>");

  while (true) {
    comparison->terms.push_back(Term::parse(lexer));

    if (ComparisonOperator::is_comparison_operator(lexer.cur_token->type))
      comparison->operators.push_back(ComparisonOperator::parse(lexer));
    else
      break;
  }

  comparison->finish_parsing(lexer, "</comparison>");
  return comparison;
}

llvm::Value *Comparison::compile(Compiler &compiler) const {
  // TODO: some operator logic (this->operators)
  // for (std::unique_ptr<Term> const &term : this->terms)
  return this->terms.at(0)->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
