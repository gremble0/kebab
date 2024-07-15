#include <cassert>
#include <vector>

#include "lexer/Token.hpp"
#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "parser/Primary.hpp"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

// TODO: errors on no argument func call - `def a = int(my-func())`

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
  while (true) {
    arguments->arguments.push_back(Expression::parse(lexer));

    if (lexer.peek()->type == Token::Type::RPAREN)
      break;
    else
      lexer.skip({Token::Type::COMMA});
  }
  lexer.skip({Token::Type::RPAREN});

  arguments->finish_parsing(lexer, "</primary-arguments>");
  return arguments;
}

llvm::Value *PrimaryArguments::compile(Compiler &compiler) const {
  std::vector<llvm::Value *> arguments_compiled;
  for (std::unique_ptr<Expression> const &argument : this->arguments)
    arguments_compiled.push_back(argument->compile(compiler));

  return compiler.builder.CreateCall(static_cast<llvm::Function *>(this->subscriptee),
                                     arguments_compiled);
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
  llvm::Value *atom_compiled = this->atom->compile(compiler);

  if (this->suffixes.size() > 0) {
    // TODO: This does not compile multiple suffixes
    this->suffixes.front()->subscriptee = atom_compiled;
    return this->suffixes.front()->compile(compiler);
  }

  return atom_compiled;
}

} // namespace Parser
} // namespace Kebab
