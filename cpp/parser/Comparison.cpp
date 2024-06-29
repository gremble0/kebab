#include <cassert>

#include "Comparison.hpp"
#include "Term.hpp"
#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<ComparisonOperator> ComparisonOperator::parse(Lexer &lexer) {
  start_parsing("comparison-operator");
  std::unique_ptr<ComparisonOperator> operator_ = std::make_unique<ComparisonOperator>();

  switch (lexer.cur_token.type) {
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
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  lexer.advance();

  end_parsing("comparison-operator");
  return operator_;
}

std::unique_ptr<Comparison> Comparison::parse(Lexer &lexer) {
  start_parsing("comparison");
  std::unique_ptr<Comparison> comparison = std::make_unique<Comparison>();

  while (true) {
    comparison->terms.push_back(Term::parse(lexer));

    if (ComparisonOperator::is_comparison_operator(lexer.cur_token.type))
      comparison->operators.push_back(ComparisonOperator::parse(lexer));
    else
      break;
  }

  end_parsing("comparison");
  return comparison;
}

} // namespace Parser
} // namespace Kebab
