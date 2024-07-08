#include <cassert>
#include <optional>

#include "logging/Logger.hpp"
#include "parser/Factor.hpp"
#include "parser/Primary.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<FactorPrefix> FactorPrefix::parse(Lexer &lexer) {
  std::unique_ptr<FactorPrefix> prefix = std::make_unique<FactorPrefix>();
  prefix->start_parsing(lexer, "<factor-prefix>");

  switch (lexer.cur_token->type) {
  case Token::Type::PLUS:
    prefix->type = FactorPrefix::Type::PLUS;
    break;

  case Token::Type::MINUS:
    prefix->type = FactorPrefix::Type::MINUS;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.cur_token->to_string_short(),
                 lexer);
  }

  lexer.advance();

  prefix->finish_parsing(lexer, "</factor-prefix>");
  return prefix;
}

llvm::Value *FactorPrefix::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FactorPrefix::compile");
}

std::unique_ptr<Factor> Factor::parse(Lexer &lexer) {
  std::unique_ptr<Factor> factor = std::make_unique<Factor>();
  factor->start_parsing(lexer, "<factor>");

  while (true) {
    if (FactorPrefix::is_factor_prefix(lexer.cur_token->type))
      factor->prefixes.push_back(FactorPrefix::parse(lexer));
    else
      factor->prefixes.push_back(std::nullopt);

    factor->primaries.push_back(Primary::parse(lexer));

    if (!FactorOperator::is_factor_operator(lexer.cur_token->type))
      break;
    factor->operators.push_back(FactorOperator::parse(lexer));
  }

  factor->finish_parsing(lexer, "</factor>");
  return factor;
}

llvm::Value *Factor::compile(Compiler &compiler) const {
  // TODO: some operator logic (this->operators)
  // TODO: some prefix logic (this->prefixes)
  // for (std::unique_ptr<Primary> const &primary : this->primaries)
  return primaries.at(0)->compile(compiler);
}

std::unique_ptr<FactorOperator> FactorOperator::parse(Lexer &lexer) {
  std::unique_ptr<FactorOperator> operator_ = std::make_unique<FactorOperator>();
  operator_->start_parsing(lexer, "<factor-operator>");

  switch (lexer.cur_token->type) {
  case Token::Type::MULT:
    operator_->type = FactorOperator::Type::MULT;
    break;

  case Token::Type::DIV:
    operator_->type = FactorOperator::Type::DIV;
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.cur_token->to_string_short(),
                 lexer);
  }

  lexer.advance();

  operator_->finish_parsing(lexer, "</factor-operator>");
  return operator_;
}

llvm::Value *FactorOperator::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FactorOperator::compile");
}

} // namespace Parser
} // namespace Kebab
