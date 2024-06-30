#ifndef KEBAB_PRIMARY_HPP
#define KEBAB_PRIMARY_HPP

#include <vector>

#include "Atom.hpp"
#include "lexer/Lexer.hpp"
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
};

class PrimarySubscription : public PrimarySuffix {
public:
  std::unique_ptr<Expression> subscription;

  static std::unique_ptr<PrimarySubscription> parse(Lexer &lexer);
};

class PrimaryArguments : public PrimarySuffix {
public:
  std::vector<std::unique_ptr<Expression>> arguments;

  static std::unique_ptr<PrimaryArguments> parse(Lexer &lexer);
};

class Primary : public AstNode {
public:
  std::unique_ptr<Atom> atom;
  std::vector<std::unique_ptr<PrimarySuffix>> suffixes;

  static std::unique_ptr<Primary> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
