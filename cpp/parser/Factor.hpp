#ifndef KEBAB_FACTOR_HPP
#define KEBAB_FACTOR_HPP

#include <optional>
#include <vector>

#include "Primary.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

// TODO: Operator interface - is_operator(type)
class FactorOperator : public AstNode /*, Operator */ {
public:
  enum Type {
    MULT,
    DIV,
  } type;

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
  void compile(Compiler::Compiler &compiler);
};

class FactorPrefix : public AstNode {
public:
  enum Type {
    PLUS,
    MINUS,
  } type;

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
  void compile(Compiler::Compiler &compiler);
};

class Factor : public AstNode {
public:
  std::vector<std::optional<std::unique_ptr<FactorPrefix>>> prefixes;
  std::vector<std::unique_ptr<Primary>> primaries;
  std::vector<std::unique_ptr<FactorOperator>> operators;

  static std::unique_ptr<Factor> parse(Lexer &lexer);
  void compile(Compiler::Compiler &compiler);
};

} // namespace Parser
} // namespace Kebab

#endif
