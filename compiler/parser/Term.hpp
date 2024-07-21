#ifndef KEBAB_TERM_HPP
#define KEBAB_TERM_HPP

#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/AstNode.hpp"
#include "parser/Factor.hpp"

namespace Kebab {
namespace Parser {

// this is a prefix?? dont remember should look into and maybe rename
class TermOperator : AstNode {
private:
  std::string to_string() const {
    switch (this->type) {
    case PLUS:
      return "+";
    case MINUS:
      return "-";
    }
  }

public:
  enum Type {
    PLUS,
    MINUS,
  } type;
  llvm::Value *lhs;
  llvm::Value *rhs;

  static constexpr bool is_term_operator(Token::Type type) {
    switch (type) {
    case Token::PLUS:
    case Token::MINUS:
      return true;

    default:
      return false;
    }
  }

  static std::unique_ptr<TermOperator> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

class Term : public AstNode {
public:
  std::vector<std::unique_ptr<Factor>> factors;
  std::vector<std::unique_ptr<TermOperator>> operators;

  static std::unique_ptr<Term> parse(Lexer &lexer);
  llvm::Value *compile(Compiler &compiler) const override;
};

} // namespace Parser
} // namespace Kebab

#endif
