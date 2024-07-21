#include <cassert>

#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Term.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<ComparisonOperator> ComparisonOperator::parse(Lexer &lexer) {
  std::unique_ptr<ComparisonOperator> operator_ = std::make_unique<ComparisonOperator>();
  operator_->start_parsing(lexer, "<comparison-operator>");

  switch (lexer.peek()->type) {
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
    // TODO: bad
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</comparison-operator>");
  return operator_;
}

llvm::Value *ComparisonOperator::compile(Compiler &compiler) const {
  switch (this->type) {
  case LT:
    return compiler.create_lt(this->lhs, this->rhs);
  case LE:
    return compiler.create_le(this->lhs, this->rhs);
  case EQ:
    return compiler.create_eq(this->lhs, this->rhs);
  case NEQ:
    return compiler.create_neq(this->lhs, this->rhs);
  case GT:
    return compiler.create_gt(this->lhs, this->rhs);
  case GE:
    return compiler.create_ge(this->lhs, this->rhs);
  }
}

std::unique_ptr<Comparison> Comparison::parse(Lexer &lexer) {
  std::unique_ptr<Comparison> comparison = std::make_unique<Comparison>();
  comparison->start_parsing(lexer, "<comparison>");

  while (true) {
    comparison->terms.push_back(Term::parse(lexer));

    if (ComparisonOperator::is_comparison_operator(lexer.peek()->type))
      comparison->operators.push_back(ComparisonOperator::parse(lexer));
    else
      break;
  }

  comparison->finish_parsing(lexer, "</comparison>");
  return comparison;
}

llvm::Value *Comparison::compile(Compiler &compiler) const {
  llvm::Value *result = this->terms.front()->compile(compiler);

  for (size_t i = 0; i < this->operators.size(); ++i) {
    llvm::Value *rhs = this->terms[i + 1]->compile(compiler);
    this->operators[i]->lhs = result;
    this->operators[i]->rhs = rhs;
    result = this->operators[i]->compile(compiler);
  }

  return result;
}

} // namespace Parser
} // namespace Kebab