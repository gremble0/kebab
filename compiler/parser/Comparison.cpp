#include <cassert>
#include <variant>
#include <vector>

#include "compiler/Errors.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Comparison.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"
#include "parser/Term.hpp"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

std::unique_ptr<ComparisonOperator> ComparisonOperator::parse(Lexer &lexer) {
  auto operator_ = std::make_unique<ComparisonOperator>();
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
    AstNode::parser_error(std::string("reached unreachable branch with token: ") +
                              lexer.peek()->to_string_short(),
                          lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</comparison-operator>");
  return operator_;
}

llvm::Value *ComparisonOperator::compile(Compiler &compiler) const {
  auto create_operation = [this, &compiler]() -> std::variant<llvm::Value *, BinaryOperatorError> {
    switch (this->type) {
    case Type::LT:
      return compiler.create_lt(this->lhs, this->rhs);

    case Type::LE:
      return compiler.create_le(this->lhs, this->rhs);

    case Type::EQ:
      return compiler.create_eq(this->lhs, this->rhs);

    case Type::NEQ:
      return compiler.create_neq(this->lhs, this->rhs);

    case Type::GT:
      return compiler.create_gt(this->lhs, this->rhs);

    case Type::GE:
      return compiler.create_ge(this->lhs, this->rhs);
    }
  };

  std::variant<llvm::Value *, BinaryOperatorError> operation = create_operation();
  if (std::holds_alternative<llvm::Value *>(operation))
    return std::get<llvm::Value *>(operation);
  else
    this->compiler_error(std::get<BinaryOperatorError>(operation));
}

std::unique_ptr<Comparison> Comparison::parse(Lexer &lexer) {
  auto comparison = std::make_unique<Comparison>();
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
