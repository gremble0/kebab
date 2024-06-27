#include <cassert>

#include "Factor.hpp"
#include "parser/Primary.hpp"

FactorPrefix *FactorPrefix::parse(Lexer &lexer) {
  start_parsing("factor-prefix");
  FactorPrefix *prefix = new FactorPrefix();

  switch (lexer.cur_token.kind) {
  case Token::Kind::PLUS:
    prefix->type = FactorPrefix::Type::PLUS;
    break;

  case Token::Kind::MINUS:
    prefix->type = FactorPrefix::Type::MINUS;
    break;

  default:
    // Unreachable
    assert(false);
  }

  end_parsing("factor-prefix");
  return prefix;
}

Factor *Factor::parse(Lexer &lexer) {
  start_parsing("factor");
  Factor *factor = new Factor();

  while (true) {
    if (FactorPrefix::is_factor_prefix(lexer.cur_token.kind))
      factor->prefixes.push_back(FactorPrefix::parse(lexer));
    else
      factor->prefixes.push_back(nullptr); // meh, nullptr bad

    factor->primaries.push_back(Primary::parse(lexer));

    if (FactorOperator::is_factor_operator(lexer.cur_token.kind))
      factor->operators.push_back(FactorOperator::parse(lexer));
    else
      break;
  }

  end_parsing("factor");
  return factor;
}

FactorOperator *FactorOperator::parse(Lexer &lexer) {
  start_parsing("factor-operator");
  FactorOperator *operator_ = new FactorOperator();

  switch (lexer.cur_token.kind) {
  case Token::Kind::MULT:
    operator_->type = FactorOperator::Type::MULT;
    break;

  case Token::Kind::DIV:
    operator_->type = FactorOperator::Type::DIV;
    break;

  default:
    // Unreachable
    assert(false);
  }

  end_parsing("factor-operator");
  return operator_;
}
