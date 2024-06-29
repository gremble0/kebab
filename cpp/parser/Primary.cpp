#include <cassert>

#include "Atom.hpp"
#include "Expression.hpp"
#include "Primary.hpp"
#include "lexer/Token.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<PrimarySubscription> PrimarySubscription::parse(Lexer &lexer) {
  start_parsing("primary-subscription");
  std::unique_ptr<PrimarySubscription> subscription = std::make_unique<PrimarySubscription>();

  skip(lexer, Token::Kind::LBRACKET);
  subscription->subscription = Expression::parse(lexer);
  skip(lexer, Token::Kind::RBRACKET);

  end_parsing("primary-subscription");
  return subscription;
}

std::unique_ptr<PrimaryArguments> PrimaryArguments::parse(Lexer &lexer) {
  start_parsing("primary-arguments");
  std::unique_ptr<PrimaryArguments> arguments = std::make_unique<PrimaryArguments>();

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

std::unique_ptr<PrimarySuffix> PrimarySuffix::parse(Lexer &lexer) {
  start_parsing("primary-suffix");
  std::unique_ptr<PrimarySuffix> suffix;

  switch (lexer.cur_token.kind) {
  case Token::Kind::LPAREN:
    suffix = PrimaryArguments::parse(lexer);
    break;

  case Token::Kind::LBRACKET:
    suffix = PrimarySuffix::parse(lexer);
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  end_parsing("primary-suffix");
  return suffix;
}

std::unique_ptr<Primary> Primary::parse(Lexer &lexer) {
  start_parsing("primary");
  std::unique_ptr<Primary> primary = std::make_unique<Primary>();

  primary->atom = Atom::parse(lexer);
  while (PrimarySuffix::is_primary_suffix_opener(lexer.cur_token.kind))
    primary->suffixes.push_back(PrimarySuffix::parse(lexer));

  end_parsing("primary");
  return primary;
}

} // namespace Parser
} // namespace Kebab
