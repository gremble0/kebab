#include <cassert>

#include "lexer/Token.hpp"
#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "parser/Primary.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<PrimarySubscription> PrimarySubscription::parse(Lexer &lexer) {
  start_parsing("primary-subscription");
  std::unique_ptr<PrimarySubscription> subscription = std::make_unique<PrimarySubscription>();

  skip(lexer, Token::Type::LBRACKET);
  subscription->subscription = Expression::parse(lexer);
  skip(lexer, Token::Type::RBRACKET);

  end_parsing("primary-subscription");
  return subscription;
}

llvm::Value *PrimarySubscription::compile(Compiler::Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function PrimarySubscription::compile");
}

std::unique_ptr<PrimaryArguments> PrimaryArguments::parse(Lexer &lexer) {
  start_parsing("primary-arguments");
  std::unique_ptr<PrimaryArguments> arguments = std::make_unique<PrimaryArguments>();

  skip(lexer, Token::Type::LPAREN);
  while (true) {
    arguments->arguments.push_back(Expression::parse(lexer));

    if (lexer.cur_token.type == Token::Type::RPAREN)
      break;
    else
      skip(lexer, Token::Type::COMMA);
  }
  skip(lexer, Token::Type::RPAREN);

  end_parsing("primary-arguments");
  return arguments;
}

llvm::Value *PrimaryArguments::compile(Compiler::Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function PrimaryArguments::compile");
}

std::unique_ptr<PrimarySuffix> PrimarySuffix::parse(Lexer &lexer) {
  start_parsing("primary-suffix");
  std::unique_ptr<PrimarySuffix> suffix;

  switch (lexer.cur_token.type) {
  case Token::Type::LPAREN:
    suffix = PrimaryArguments::parse(lexer);
    break;

  case Token::Type::LBRACKET:
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
  while (PrimarySuffix::is_primary_suffix_opener(lexer.cur_token.type))
    primary->suffixes.push_back(PrimarySuffix::parse(lexer));

  end_parsing("primary");
  return primary;
}

llvm::Value *Primary::compile(Compiler::Compiler &compiler) const {
  // TODO: some suffix logic (this->suffixes)
  this->atom->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
