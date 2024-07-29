#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <vector>

#include "lexer/Token.hpp"
#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "parser/Primary.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab::Parser {

std::unique_ptr<PrimarySubscription> PrimarySubscription::parse(Lexer &lexer) {
  auto subscription = std::make_unique<PrimarySubscription>();
  subscription->start_parsing(lexer, "<primary-subscription>");

  lexer.skip({Token::Type::LBRACKET});
  subscription->subscription = Expression::parse(lexer);
  lexer.skip({Token::Type::RBRACKET});

  subscription->finish_parsing(lexer, "</primary-subscription>");
  return subscription;
}

llvm::Value *PrimarySubscription::compile(Compiler &compiler) const {
  llvm::Value *offset = this->subscription->compile(compiler);
  if (offset->getType() != compiler.get_int_type())
    // TODO: could be more descriptive (arrays can only be subscripted with ints)
    this->type_error({compiler.get_int_type()}, offset->getType());

  // Variable lookups are stored as LoadInstructions
  llvm::LoadInst *load;
  if (load = llvm::dyn_cast<llvm::LoadInst>(this->subscriptee); load == nullptr)
    this->unsubscriptable_error(this->subscriptee->getType());

  llvm::Value *pointee = load->getPointerOperand();
  llvm::AllocaInst *alloca;
  // Subscriptable values are all stack allocated with alloca
  if (alloca = llvm::dyn_cast<llvm::AllocaInst>(pointee); alloca == nullptr)
    this->unsubscriptable_error(pointee->getType());

  // TODO: raise unsubscriptable_error here if its not an array allocation. Does not work atm, just
  // segfaults if attempt to index non array.

  std::optional<llvm::Value *> subscription_compiled = compiler.create_subscription(alloca, offset);
  if (subscription_compiled.has_value())
    return subscription_compiled.value();
  else
    this->index_error(alloca->getArraySize(), offset);
}

std::unique_ptr<PrimaryArguments> PrimaryArguments::parse(Lexer &lexer) {
  auto arguments = std::make_unique<PrimaryArguments>();
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
  for (const std::unique_ptr<Expression> &argument : this->arguments)
    arguments_compiled.push_back(argument->compile(compiler));

  if (auto *function = llvm::dyn_cast<llvm::Function>(this->subscriptee))
    return compiler.create_call(function, arguments_compiled);
  else
    this->uncallable_error(this->subscriptee->getType());
}

std::unique_ptr<PrimarySuffix> PrimarySuffix::parse(Lexer &lexer) {
  std::unique_ptr<PrimarySuffix> suffix;

  switch (lexer.peek()->type) {
  case Token::Type::LPAREN:
    suffix = PrimaryArguments::parse(lexer);
    break;

  case Token::Type::LBRACKET:
    suffix = PrimarySubscription::parse(lexer);
    break;

  default:
    AstNode::parser_error(std::string("reached unreachable branch with token: ") +
                              lexer.peek()->to_string_short(),
                          lexer);
  }

  return suffix;
}

std::unique_ptr<Primary> Primary::parse(Lexer &lexer) {
  auto primary = std::make_unique<Primary>();
  primary->start_parsing(lexer, "<primary>");

  primary->atom = Atom::parse(lexer);
  while (PrimarySuffix::is_primary_suffix_opener(lexer.peek()->type))
    primary->suffixes.push_back(PrimarySuffix::parse(lexer));

  primary->finish_parsing(lexer, "</primary>");
  return primary;
}

llvm::Value *Primary::compile(Compiler &compiler) const {
  llvm::Value *result = this->atom->compile(compiler);

  std::ranges::for_each(this->suffixes, [&result, &compiler](auto &suffix) {
    suffix->subscriptee = result;
    result = suffix->compile(compiler);
  });

  return result;
}

} // namespace Kebab::Parser
