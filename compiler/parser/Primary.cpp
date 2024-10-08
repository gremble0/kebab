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
  llvm::Value *index = this->subscription->compile(compiler);
  // Subscriptions can only be done with ints
  if (auto error = TypeError::check(compiler.get_int_type(), index->getType()); error.has_value())
    this->compiler_error(error.value());

  // Only certain types can be subscripted
  if (auto error = UnsubscriptableError::check(this->subscriptee); error.has_value())
    this->compiler_error(error.value());

  // Dont index if out of range (currently unimplemented, and should maybe be removed since we
  // cannot check this at compiletime with heap allocated lists)
  if (auto error = IndexError::check(this->subscriptee, index); error.has_value())
    this->compiler_error(error.value());

  return compiler.create_subscription(this->subscriptee, index);
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

  if (auto error = UncallableError::check(this->subscriptee); error.has_value())
    this->compiler_error(error.value());

  if (auto *function = llvm::dyn_cast<llvm::Function>(this->subscriptee); function != nullptr) {
    std::variant<llvm::CallInst *, ArgumentCountError> call =
        compiler.create_call(function, arguments_compiled);
    if (std::holds_alternative<ArgumentCountError>(call))
      this->compiler_error(std::get<ArgumentCountError>(call));
    else
      return std::get<llvm::CallInst *>(call);
  } else {
    // This means the variable is a function pointer - need to get the type of the pointed to
    // function
    assert(false && "calling from function pointers is so hard to do with opaque pointers, haven't "
                    "figured out yet");
  }
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
