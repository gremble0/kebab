#ifndef KEBAB_ATOM_HPP
#define KEBAB_ATOM_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

class Expression;

class Atom : public AstNode {
public:
  virtual ~Atom() = default;

  static std::unique_ptr<Atom> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler::Compiler &compiler) const = 0;
};

class IntAtom : public Atom {
public:
  int64_t i;

  static std::unique_ptr<IntAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class FloatAtom : public Atom {
public:
  float_t f;

  static std::unique_ptr<FloatAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class CharAtom : public Atom {
public:
  uint8_t c;

  static std::unique_ptr<CharAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class StringAtom : public Atom {
public:
  std::string s;

  static std::unique_ptr<StringAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class BoolAtom : public Atom {
public:
  bool b;

  static std::unique_ptr<BoolAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class NameAtom : public Atom {
public:
  std::string name;

  static std::unique_ptr<NameAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class InnerExpressionAtom : public Atom {
public:
  std::unique_ptr<Expression> expression;

  static std::unique_ptr<InnerExpressionAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

class ListAtom : public Atom {
public:
  std::vector<std::unique_ptr<Expression>> list;

  static std::unique_ptr<ListAtom> parse(Lexer &lexer);
  llvm::Value *compile(Compiler::Compiler &compiler) const;
};

} // namespace Parser
} // namespace Kebab

#endif
