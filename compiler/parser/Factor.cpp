#include <cassert>
#include <optional>

#include "parser/Factor.hpp"
#include "parser/Primary.hpp"

namespace Kebab::Parser {

std::unique_ptr<FactorPrefix> FactorPrefix::parse(Lexer &lexer) {
  std::unique_ptr<FactorPrefix> prefix = std::make_unique<FactorPrefix>();
  prefix->start_parsing(lexer, "<factor-prefix>");

  switch (lexer.peek()->type) {
  case Token::Type::PLUS:
    prefix->type = FactorPrefix::Type::PLUS;
    break;

  case Token::Type::MINUS:
    prefix->type = FactorPrefix::Type::MINUS;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  lexer.advance();

  prefix->finish_parsing(lexer, "</factor-prefix>");
  return prefix;
}

llvm::Value *FactorPrefix::compile(Compiler &compiler) const {
  std::optional<llvm::Value *> operation;

  switch (this->type) {
  case Type::PLUS:
    operation = this->prefixed;

  case Type::MINUS:
    operation = compiler.create_neg(this->prefixed);
  }

  if (operation.has_value())
    return operation.value();
  else
    this->operator_error({compiler.get_int_type()}, this->prefixed->getType(), this->to_string());
}

std::unique_ptr<Factor> Factor::parse(Lexer &lexer) {
  std::unique_ptr<Factor> factor = std::make_unique<Factor>();
  factor->start_parsing(lexer, "<factor>");

  while (true) {
    if (FactorPrefix::is_factor_prefix(lexer.peek()->type))
      factor->prefixes.push_back(FactorPrefix::parse(lexer));
    else
      factor->prefixes.push_back(std::nullopt);

    factor->primaries.push_back(Primary::parse(lexer));

    if (!FactorOperator::is_factor_operator(lexer.peek()->type))
      break;
    factor->operators.push_back(FactorOperator::parse(lexer));
  }

  factor->finish_parsing(lexer, "</factor>");
  return factor;
}

llvm::Value *Factor::compile_with_prefix(Compiler &compiler, size_t index) const {
  llvm::Value *primary = this->primaries[index]->compile(compiler);
  if (this->prefixes[index].has_value()) {
    this->prefixes[index]->get()->prefixed = primary;
    primary = this->prefixes[index]->get()->compile(compiler);
  }
  return primary;
}

llvm::Value *Factor::compile(Compiler &compiler) const {
  llvm::Value *result = this->compile_with_prefix(compiler, 0);

  for (size_t i = 0; i < this->operators.size(); ++i) {
    llvm::Value *rhs = this->compile_with_prefix(compiler, i + 1);
    this->operators[i]->lhs = result;
    this->operators[i]->rhs = rhs;
    result = this->operators[i]->compile(compiler);
  }

  return result;
}

std::unique_ptr<FactorOperator> FactorOperator::parse(Lexer &lexer) {
  std::unique_ptr<FactorOperator> operator_ = std::make_unique<FactorOperator>();
  operator_->start_parsing(lexer, "<factor-operator>");

  switch (lexer.peek()->type) {
  case Token::Type::MULT:
    operator_->type = FactorOperator::Type::MULT;
    break;

  case Token::Type::DIV:
    operator_->type = FactorOperator::Type::DIV;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</factor-operator>");
  return operator_;
}

llvm::Value *FactorOperator::compile(Compiler &compiler) const {
  std::optional<llvm::Value *> operation;
  switch (this->type) {
  case Type::MULT:
    operation = compiler.create_mul(this->lhs, this->rhs);
    break;

  case Type::DIV:
    operation = compiler.create_div(this->lhs, this->rhs);
    break;
  }

  if (operation.has_value())
    return operation.value();
  else
    this->operator_error({compiler.get_int_type(), compiler.get_float_type()}, this->lhs->getType(),
                         this->to_string());
}

} // namespace Kebab::Parser
