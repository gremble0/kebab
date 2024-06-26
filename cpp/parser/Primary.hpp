#ifndef KEBAB_PRIMARY_HPP
#define KEBAB_PRIMARY_HPP

#include <vector>

#include "Atom.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class PrimarySuffix : AstNode {
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
};

class Primary : AstNode {
public:
  Atom atom;
  std::vector<PrimarySuffix *> suffixes;

  static Primary *parse(Lexer &lexer);
};

#endif
