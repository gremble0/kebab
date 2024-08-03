#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <memory>
#include <string>

#include "compiler/Compiler.hpp"
#include "compiler/Errors.hpp"
#include "lexer/Lexer.hpp"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

class AstNode {
private:
  Span span;
  std::string path;

  static std::string getnline(const std::string &path, size_t line_number);

  std::string where() const;

protected:
  AstNode() = default;

  [[noreturn]] static void parser_error(const std::string &message, const Lexer &lexer);
  [[noreturn]] void unreachable_error() const;
  [[noreturn]] void nonhomogenous_list_error() const;
  [[noreturn]] void uncallable_error(const UncallableError &error) const;
  [[noreturn]] void argument_count_error(const ArgumentCountError &error) const;
  [[noreturn]] void immutable_assignment_error(const std::string &name) const;
  [[noreturn]] void unsubscriptable_error(const llvm::Type *subscriptee) const;
  [[noreturn]] void index_error(const llvm::Value *capacity, const llvm::Value *index) const;
  [[noreturn]] void reassignment_error(const std::string &name) const;
  [[noreturn]] void assign_nonexisting_error(const std::string &name) const;
  [[noreturn]] void unrecognized_type_error(const std::string &type_name) const;
  [[noreturn]] void name_error(const std::string &name) const;
  [[noreturn]] void type_error(std::initializer_list<const llvm::Type *> expected,
                               const llvm::Type *actual) const;
  [[noreturn]] void operator_error(const std::vector<const llvm::Type *> &supported,
                                   const llvm::Type *actual, const std::string &operator_) const;
  [[noreturn]] void operator_error(const std::vector<const llvm::Type *> &supported,
                                   const llvm::Type *lhs, const llvm::Type *rhs,
                                   const std::string &operator_) const;

  void start_parsing(const Lexer &lexer, const std::string &node_name);
  void finish_parsing(const Lexer &lexer, const std::string &node_name);

public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const = 0;
};

} // namespace Kebab::Parser

#endif
