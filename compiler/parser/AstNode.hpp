#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <memory>
#include <string>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

class AstNode {
private:
  /**
   * @brief get the nth line from a file given by path
   */
  static std::string getnline(const std::string &path, size_t line_number);

  std::string where() const;

protected:
  Span span;
  std::string path;

  AstNode() = default;

  [[noreturn]] static void parser_error(const std::string &message, Lexer &lexer);
  [[noreturn]] void unreachable_error() const;
  [[noreturn]] void unrecognized_type_error(const std::string &type_name) const;
  [[noreturn]] void name_error(const std::string &name) const;
  [[noreturn]] void type_error(std::initializer_list<const llvm::Type *> expected,
                               const llvm::Type *actual) const;
  [[noreturn]] void operator_error(std::vector<const llvm::Type *> supported,
                                   const llvm::Type *actual, const std::string &operator_) const;
  [[noreturn]] void operator_error(std::vector<const llvm::Type *> supported, const llvm::Type *lhs,
                                   const llvm::Type *rhs, const std::string &operator_) const;

  void start_parsing(Lexer &lexer, const std::string &node_name);
  void finish_parsing(Lexer &lexer, const std::string &node_name);

public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const = 0;
};

} // namespace Parser
} // namespace Kebab

#endif
