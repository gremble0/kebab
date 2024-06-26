// TODO: KEBAB_ before #ifndef everywhere
#ifndef KEBAB_ATOM_HPP
#define KEBAB_ATOM_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class Expression;

class Atom : public AstNode {
public:
  static Atom *parse(Lexer &lexer);
};

class CharAtom : public Atom {
public:
  uint8_t c;

  static CharAtom *parse(Lexer &lexer);
};

class StringAtom : public Atom {
public:
  std::string s;

  static StringAtom *parse(Lexer &lexer);
};

class BoolAtom : public Atom {
public:
  bool b;

  static BoolAtom *parse(Lexer &lexer);
};

class NameAtom : public Atom {
public:
  std::string name;

  static NameAtom *parse(Lexer &lexer);
};

class InnerExpressionAtom : public Atom {
public:
  Expression *expression;

  static InnerExpressionAtom *parse(Lexer &lexer);
};

class ListAtom : public Atom {
public:
  std::vector<Expression *> list;

  static ListAtom *parse(Lexer &lexer);
};

#endif
