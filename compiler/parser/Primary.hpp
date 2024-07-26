#ifndef KEBAB_PRIMARY_HPP
#define KEBAB_PRIMARY_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Atom.hpp"

namespace Kebab::Parser {

class PrimarySuffix : public AstNode {
public:
  llvm::Value *subscriptee;

  ~PrimarySuffix() override = default;

  static constexpr bool is_primary_suffix_opener(Token::Type type) {
    switch (type) {
    case Token::Type::LBRACKET:
    case Token::Type::LPAREN:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<PrimarySuffix> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override = 0;
};

class PrimarySubscription : public PrimarySuffix {
public:
  std::unique_ptr<Expression> subscription;

  static std::unique_ptr<PrimarySubscription> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class PrimaryArguments : public PrimarySuffix {
public:
  std::vector<std::unique_ptr<Expression>> arguments;

  static std::unique_ptr<PrimaryArguments> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class Primary : public AstNode {
public:
  std::unique_ptr<Atom> atom;
  std::vector<std::unique_ptr<PrimarySuffix>> suffixes;

  static std::unique_ptr<Primary> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

} // namespace Kebab::Parser

#endif
