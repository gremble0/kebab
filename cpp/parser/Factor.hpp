#ifndef KEBAB_FACTOR_HPP
#define KEBAB_FACTOR_HPP

#include <optional>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Primary.hpp"

namespace Kebab {
namespace Parser {

class FactorOperator : public AstNode {
private:
  std::string to_string() const {
    if (this->type == MULT)
      return "*";
    else if (this->type == DIV)
      return "/";

    this->unreachable_error();
  }

public:
  enum Type {
    MULT,
    DIV,
  } type;
  llvm::Value *lhs;
  llvm::Value *rhs;

  static constexpr bool is_factor_operator(Token::Type type) {
    switch (type) {
    case Token::DIV:
    case Token::MULT:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<FactorOperator> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class FactorPrefix : public AstNode {
public:
  enum Type {
    PLUS,
    MINUS,
  } type;
  llvm::Value *prefixed;

  static constexpr bool is_factor_prefix(Token::Type type) {
    switch (type) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<FactorPrefix> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class Factor : public AstNode {
public:
  std::vector<std::optional<std::unique_ptr<FactorPrefix>>> prefixes;
  std::vector<std::unique_ptr<Primary>> primaries;
  std::vector<std::unique_ptr<FactorOperator>> operators;

  static std::unique_ptr<Factor> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

} // namespace Parser
} // namespace Kebab

#endif
