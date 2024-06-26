#ifndef KEBAB_KEBAB_NOTTEST_HPP
#define KEBAB_KEBAB_NOTTEST_HPP

#include "Comparison.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class NotTest : AstNode {
public:
  bool is_negated;
  Comparison *comparison;

  static NotTest *parse(Lexer &lexer);
};

#endif
