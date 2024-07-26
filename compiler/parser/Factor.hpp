#ifndef KEBAB_FACTOR_HPP
#define KEBAB_FACTOR_HPP

#include <optional>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Primary.hpp"

namespace Kebab::Parser {

class FactorOperator : public AstNode {
private:
  std::string to_string() const {
    switch (this->type) {
    case Type::MULT:
      return "*";
    case Type::DIV:
      return "/";
    }
  }

public:
  enum class Type {
    MULT,
    DIV,
  };
  Type type;
  llvm::Value *lhs;
  llvm::Value *rhs;

  static constexpr bool is_factor_operator(Token::Type type) {
    switch (type) {
    case Token::Type::DIV:
    case Token::Type::MULT:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<FactorOperator> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class FactorPrefix : public AstNode {
private:
  std::string to_string() const {
    switch (this->type) {
    case Type::PLUS:
      return "*";
    case Type::MINUS:
      return "/";
    }
  }

public:
  enum class Type {
    PLUS,
    MINUS,
  };
  Type type;
  llvm::Value *prefixed;

  static constexpr bool is_factor_prefix(Token::Type type) {
    switch (type) {
    case Token::Type::PLUS:
    case Token::Type::MINUS:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<FactorPrefix> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

class Factor : public AstNode {
private:
  llvm::Value *compile_with_prefix(Compiler &compiler, size_t index) const;

public:
  std::vector<std::optional<std::unique_ptr<FactorPrefix>>> prefixes;
  std::vector<std::unique_ptr<Primary>> primaries;
  std::vector<std::unique_ptr<FactorOperator>> operators;

  static std::unique_ptr<Factor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const final;
};

} // namespace Kebab::Parser

#endif
