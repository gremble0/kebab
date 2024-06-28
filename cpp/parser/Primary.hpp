#ifndef KEBAB_PRIMARY_HPP
#define KEBAB_PRIMARY_HPP

#include <vector>

#include "Atom.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Parser {

class PrimarySuffix : public AstNode {
public:
  static constexpr bool is_primary_suffix_opener(Token::Kind kind) {
    switch (kind) {
    case Token::LBRACKET:
    case Token::LPAREN:
      return true;

    default:
      return false;
    }
  }

  static PrimarySuffix *parse(Lexer &lexer);
};

class PrimarySubscription : public PrimarySuffix {
public:
  Expression *subscription;

  static PrimarySubscription *parse(Lexer &lexer);
};

class PrimaryArguments : public PrimarySuffix {
public:
  std::vector<Expression *> arguments;

  static PrimaryArguments *parse(Lexer &lexer);
};

class Primary : public AstNode {
public:
  Atom *atom;
  std::vector<PrimarySuffix *> suffixes;

  static Primary *parse(Lexer &lexer);
};

} // namespace Parser

#endif
