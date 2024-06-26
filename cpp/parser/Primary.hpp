#ifndef PRIMARY_HPP
#define PRIMARY_HPP

#include <vector>

#include "Atom.hpp"
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
};

#endif
