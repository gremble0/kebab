#ifndef KEBAB_ATOM_HPP
#define KEBAB_ATOM_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"

namespace Kebab::Parser {

class Expression;

class Atom : public AstNode {
public:
  ~Atom() override = default;

  static std::unique_ptr<Atom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override = 0;
};

class IntAtom : public Atom {
public:
  int64_t i;

  static std::unique_ptr<IntAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class FloatAtom : public Atom {
public:
  double_t f;

  static std::unique_ptr<FloatAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class CharAtom : public Atom {
public:
  uint8_t c;

  static std::unique_ptr<CharAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class StringAtom : public Atom {
public:
  std::string s;

  static std::unique_ptr<StringAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class BoolAtom : public Atom {
public:
  bool b;

  static std::unique_ptr<BoolAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class NameAtom : public Atom {
public:
  std::string name;

  static std::unique_ptr<NameAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class InnerExpressionAtom : public Atom {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<InnerExpressionAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class ListAtom : public Atom {
public:
  std::vector<std::unique_ptr<Expression>> list;

  static std::unique_ptr<ListAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

} // namespace Kebab::Parser

#endif
