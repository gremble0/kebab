#include <cassert>

#include "Atom.hpp"
#include "Expression.hpp"
#include "Primary.hpp"
#include "lexer/Token.hpp"

namespace Parser {

PrimarySubscription *PrimarySubscription::parse(Lexer &lexer) {
  start_parsing("primary-subscription");
  PrimarySubscription *subscription = new PrimarySubscription();

  skip(lexer, Token::Kind::LBRACKET);
  subscription->subscription = Expression::parse(lexer);
  skip(lexer, Token::Kind::RBRACKET);

  end_parsing("primary-subscription");
  return subscription;
}

PrimaryArguments *PrimaryArguments::parse(Lexer &lexer) {
  start_parsing("primary-arguments");
  PrimaryArguments *arguments = new PrimaryArguments();

  skip(lexer, Token::Kind::LPAREN);
  while (true) {
    arguments->arguments.push_back(Expression::parse(lexer));

    if (lexer.cur_token.kind == Token::Kind::RPAREN)
      break;
    else
      skip(lexer, Token::Kind::COMMA);
  }
  skip(lexer, Token::Kind::RPAREN);

  end_parsing("primary-arguments");
  return arguments;
}

PrimarySuffix *PrimarySuffix::parse(Lexer &lexer) {
  start_parsing("primary-suffix");
  PrimarySuffix *suffix;

  switch (lexer.cur_token.kind) {
  case Token::Kind::LPAREN:
    suffix = PrimaryArguments::parse(lexer);
    break;

  case Token::Kind::LBRACKET:
    suffix = PrimarySuffix::parse(lexer);
    break;

  default:
    // Unreachable
    assert(false);
  }

  end_parsing("primary-suffix");
  return suffix;
}

Primary *Primary::parse(Lexer &lexer) {
  start_parsing("primary");
  Primary *primary = new Primary();

  primary->atom = Atom::parse(lexer);
  while (PrimarySuffix::is_primary_suffix_opener(lexer.cur_token.kind))
    primary->suffixes.push_back(PrimarySuffix::parse(lexer));

  end_parsing("primary");
  return primary;
}
} // namespace Parser
