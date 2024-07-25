#include <cassert>

#include "lexer/Lexer.hpp"
#include "parser/Term.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<TermOperator> TermOperator::parse(Lexer &lexer) {
  std::unique_ptr<TermOperator> operator_ = std::make_unique<TermOperator>();
  operator_->start_parsing(lexer, "<term-operator>");

  switch (lexer.peek()->type) {
  case Token::Type::PLUS:
    operator_->type = TermOperator::Type::PLUS;
    break;

  case Token::Type::MINUS:
    operator_->type = TermOperator::Type::MINUS;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</term-operator>");
  return operator_;
}

llvm::Value *TermOperator::compile(Compiler &compiler) const {
  std::optional<llvm::Value *> operation;

  switch (this->type) {
  case Type::PLUS:
    operation = compiler.create_add(this->lhs, this->rhs);
    break;

  case Type::MINUS:
    operation = compiler.create_sub(this->lhs, this->rhs);
    break;
  }

  if (operation.has_value())
    return operation.value();
  else
    this->operator_error({compiler.get_int_type(), compiler.get_float_type()}, this->lhs->getType(),
                         this->rhs->getType(), this->to_string());
}

std::unique_ptr<Term> Term::parse(Lexer &lexer) {
  std::unique_ptr<Term> term = std::make_unique<Term>();
  term->start_parsing(lexer, "<term>");

  while (true) {
    term->factors.push_back(Factor::parse(lexer));

    if (TermOperator::is_term_operator(lexer.peek()->type))
      term->operators.push_back(TermOperator::parse(lexer));
    else
      break;
  }

  term->finish_parsing(lexer, "</term>");
  return term;
}

llvm::Value *Term::compile(Compiler &compiler) const {
  llvm::Value *result = this->factors.front()->compile(compiler);

  for (size_t i = 0; i < this->operators.size(); ++i) {
    llvm::Value *rhs = this->factors[i + 1]->compile(compiler);
    this->operators[i]->lhs = result;
    this->operators[i]->rhs = rhs;
    result = this->operators[i]->compile(compiler);
  }

  return result;
}

} // namespace Parser
} // namespace Kebab
