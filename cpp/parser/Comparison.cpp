#include <cassert>

#include "Comparison.hpp"
#include "Term.hpp"
#include "lexer/Lexer.hpp"

namespace Kebab {
namespace Parser {

ComparisonOperator *ComparisonOperator::parse(Lexer &lexer) {
  start_parsing("comparison-operator");
  ComparisonOperator *operator_ = new ComparisonOperator();

  switch (lexer.cur_token.kind) {
  case Token::Kind::LT:
    operator_->type = ComparisonOperator::Type::LT;
    break;

  case Token::Kind::LE:
    operator_->type = ComparisonOperator::Type::LE;
    break;

  case Token::Kind::EQ:
    operator_->type = ComparisonOperator::Type::EQ;
    break;

  case Token::Kind::NEQ:
    operator_->type = ComparisonOperator::Type::NEQ;
    break;

  case Token::Kind::GT:
    operator_->type = ComparisonOperator::Type::GT;
    break;

  case Token::Kind::GE:
    operator_->type = ComparisonOperator::Type::GE;
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  lexer.advance();

  end_parsing("comparison-operator");
  return operator_;
}

Comparison *Comparison::parse(Lexer &lexer) {
  start_parsing("comparison");
  Comparison *comparison = new Comparison();

  while (true) {
    comparison->terms.push_back(Term::parse(lexer));

    if (ComparisonOperator::is_comparison_operator(lexer.cur_token.kind))
      comparison->operators.push_back(ComparisonOperator::parse(lexer));
    else
      break;
  }

  end_parsing("comparison");
  return comparison;
}

} // namespace Parser
} // namespace Kebab
