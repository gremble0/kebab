#ifndef KEBAB_PRIMARY_HPP
#define KEBAB_PRIMARY_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Atom.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class PrimarySuffix : public AstNode {
public:
  virtual ~PrimarySuffix() = default;

  static constexpr bool is_primary_suffix_opener(Token::Type type) {
    switch (type) {
    case Token::LBRACKET:
    case Token::LPAREN:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<PrimarySuffix> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler::Compiler &compiler) const = 0;
};

class PrimarySubscription : public PrimarySuffix {
public:
  std::unique_ptr<Expression> subscription;

  static std::unique_ptr<PrimarySubscription> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class PrimaryArguments : public PrimarySuffix {
public:
  std::vector<std::unique_ptr<Expression>> arguments;

  static std::unique_ptr<PrimaryArguments> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class Primary : public AstNode {
public:
  std::unique_ptr<Atom> atom;
  std::vector<std::unique_ptr<PrimarySuffix>> suffixes;

  static std::unique_ptr<Primary> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
