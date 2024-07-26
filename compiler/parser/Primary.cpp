#include <cassert>
#include <vector>

#include "lexer/Token.hpp"
#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "parser/Primary.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

std::unique_ptr<PrimarySubscription> PrimarySubscription::parse(Lexer &lexer) {
  std::unique_ptr<PrimarySubscription> subscription = std::make_unique<PrimarySubscription>();
  subscription->start_parsing(lexer, "<primary-subscription>");

  lexer.skip({Token::Type::LBRACKET});
  subscription->subscription = Expression::parse(lexer);
  lexer.skip({Token::Type::RBRACKET});

  subscription->finish_parsing(lexer, "</primary-subscription>");
  return subscription;
}

llvm::Value *PrimarySubscription::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function PrimarySubscription::compile");
}

std::unique_ptr<PrimaryArguments> PrimaryArguments::parse(Lexer &lexer) {
  std::unique_ptr<PrimaryArguments> arguments = std::make_unique<PrimaryArguments>();
  arguments->start_parsing(lexer, "<primary-arguments>");

  lexer.skip({Token::Type::LPAREN});
  while (lexer.peek()->type != Token::Type::RPAREN) {
    arguments->arguments.push_back(Expression::parse(lexer));

    lexer.expect({Token::Type::COMMA, Token::Type::RPAREN});
    lexer.try_skip({Token::Type::COMMA});
  }
  lexer.skip({Token::Type::RPAREN});

  arguments->finish_parsing(lexer, "</primary-arguments>");
  return arguments;
}

llvm::Value *PrimaryArguments::compile(Compiler &compiler) const {
  std::vector<llvm::Value *> arguments_compiled;
  for (std::unique_ptr<Expression> const &argument : this->arguments)
    arguments_compiled.push_back(argument->compile(compiler));

  return compiler.create_call(static_cast<llvm::Function *>(this->subscriptee), arguments_compiled);
}

std::unique_ptr<PrimarySuffix> PrimarySuffix::parse(Lexer &lexer) {
  std::unique_ptr<PrimarySuffix> suffix;

  switch (lexer.peek()->type) {
  case Token::Type::LPAREN:
    suffix = PrimaryArguments::parse(lexer);
    break;

  case Token::Type::LBRACKET:
    suffix = PrimarySuffix::parse(lexer);
    break;

  default:
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  return suffix;
}

std::unique_ptr<Primary> Primary::parse(Lexer &lexer) {
  std::unique_ptr<Primary> primary = std::make_unique<Primary>();
  primary->start_parsing(lexer, "<primary>");

  primary->atom = Atom::parse(lexer);
  while (PrimarySuffix::is_primary_suffix_opener(lexer.peek()->type))
    primary->suffixes.push_back(PrimarySuffix::parse(lexer));

  primary->finish_parsing(lexer, "</primary>");
  return primary;
}

llvm::Value *Primary::compile(Compiler &compiler) const {
  llvm::Value *result = this->atom->compile(compiler);

  for (size_t i = 0, size = this->suffixes.size(); i < size; ++i) {
    this->suffixes[i]->subscriptee = result;
    result = this->suffixes[i]->compile(compiler);
  }

  return result;
}

} // namespace Kebab::Parser
