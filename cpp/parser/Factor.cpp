#include <cassert>
#include <optional>

#include "Factor.hpp"
#include "parser/Primary.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<FactorPrefix> FactorPrefix::parse(Lexer &lexer) {
  start_parsing("factor-prefix");
  std::unique_ptr<FactorPrefix> prefix = std::make_unique<FactorPrefix>();

  switch (lexer.cur_token.kind) {
  case Token::Kind::PLUS:
    prefix->type = FactorPrefix::Type::PLUS;
    break;

  case Token::Kind::MINUS:
    prefix->type = FactorPrefix::Type::MINUS;
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  lexer.advance();

  end_parsing("factor-prefix");
  return prefix;
}

std::unique_ptr<Factor> Factor::parse(Lexer &lexer) {
  start_parsing("factor");
  std::unique_ptr<Factor> factor = std::make_unique<Factor>();

  while (true) {
    if (FactorPrefix::is_factor_prefix(lexer.cur_token.kind))
      factor->prefixes.push_back(FactorPrefix::parse(lexer));
    else
      factor->prefixes.push_back(std::nullopt);

    factor->primaries.push_back(Primary::parse(lexer));

    if (!FactorOperator::is_factor_operator(lexer.cur_token.kind))
      break;
    factor->operators.push_back(FactorOperator::parse(lexer));
  }

  end_parsing("factor");
  return factor;
}

std::unique_ptr<FactorOperator> FactorOperator::parse(Lexer &lexer) {
  start_parsing("factor-operator");
  std::unique_ptr<FactorOperator> operator_ = std::make_unique<FactorOperator>();

  switch (lexer.cur_token.kind) {
  case Token::Kind::MULT:
    operator_->type = FactorOperator::Type::MULT;
    break;

  case Token::Kind::DIV:
    operator_->type = FactorOperator::Type::DIV;
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  lexer.advance();

  end_parsing("factor-operator");
  return operator_;
}

} // namespace Parser
} // namespace Kebab
