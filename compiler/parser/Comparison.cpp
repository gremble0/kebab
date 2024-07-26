#include <cassert>
#include <initializer_list>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"
#include "parser/Term.hpp"

namespace Kebab::Parser {

std::unique_ptr<ComparisonOperator> ComparisonOperator::parse(Lexer &lexer) {
  std::unique_ptr<ComparisonOperator> operator_ = std::make_unique<ComparisonOperator>();
  operator_->start_parsing(lexer, "<comparison-operator>");

  switch (lexer.peek()->type) {
    using enum Token::Type;
  case LT:
    operator_->type = ComparisonOperator::Type::LT;
    break;

  case LE:
    operator_->type = ComparisonOperator::Type::LE;
    break;

  case EQ:
    operator_->type = ComparisonOperator::Type::EQ;
    break;

  case NEQ:
    operator_->type = ComparisonOperator::Type::NEQ;
    break;

  case GT:
    operator_->type = ComparisonOperator::Type::GT;
    break;

  case GE:
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
  std::optional<llvm::Value *> operation;
  std::vector<const llvm::Type *> supported_types = {compiler.get_int_type(),
                                                     compiler.get_float_type()};

  switch (this->type) {
  case Type::LT:
    operation = compiler.create_lt(this->lhs, this->rhs);
    break;

  case Type::LE:
    operation = compiler.create_le(this->lhs, this->rhs);
    break;

  case Type::EQ:
    operation = compiler.create_eq(this->lhs, this->rhs);
    supported_types.push_back(compiler.get_bool_type());
    break;

  case Type::NEQ:
    operation = compiler.create_neq(this->lhs, this->rhs);
    break;

  case Type::GT:
    operation = compiler.create_gt(this->lhs, this->rhs);
    break;

  case Type::GE:
    operation = compiler.create_ge(this->lhs, this->rhs);
    break;
  }

  if (operation.has_value())
    return operation.value();
  else
    this->operator_error(supported_types, this->lhs->getType(), this->rhs->getType(),
                         this->to_string());
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

} // namespace Kebab::Parser
