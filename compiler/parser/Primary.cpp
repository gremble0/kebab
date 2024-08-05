#include <algorithm>
#include <cassert>
#include <optional>
#include <variant>
#include <vector>

#include "compiler/Errors.hpp"
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
  // TODO: do these error checks elsewhere
  // TODO: could be more descriptive (arrays can only be subscripted with ints)
  if (auto maybe_error = TypeError::check({compiler.get_int_type()}, offset->getType());
      maybe_error.has_value())
    this->compiler_error(maybe_error.value());

  if (auto maybe_error = UnsubscriptableError::check(this->subscriptee); maybe_error.has_value())
    this->compiler_error(maybe_error.value());

  // TODO: improve casting
  // Variable lookups are stored as LoadInstructions
  llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(this->subscriptee);

  llvm::Value *pointee = load->getPointerOperand();
  // Subscriptable values are all stack allocated with alloca
  llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(pointee);

  std::variant<llvm::Value *, IndexError> subscription_compiled =
      compiler.create_subscription(alloca, offset);
  if (std::holds_alternative<llvm::Value *>(subscription_compiled))
    return std::get<llvm::Value *>(subscription_compiled);
  else
    this->compiler_error(std::get<IndexError>(subscription_compiled));
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

  if (auto maybe_error = UncallableError::check(this->subscriptee); maybe_error.has_value())
    this->compiler_error(maybe_error.value());

  auto *function = llvm::dyn_cast<llvm::Function>(this->subscriptee);
  std::variant<llvm::CallInst *, ArgumentCountError> call =
      compiler.create_call(function, arguments_compiled);
  if (std::holds_alternative<ArgumentCountError>(call))
    this->compiler_error(std::get<ArgumentCountError>(call));
  else
    return std::get<llvm::CallInst *>(call);
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
