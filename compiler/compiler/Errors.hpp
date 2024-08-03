#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include <cstddef>
#include <optional>

#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"

class CompilerError {
public:
  virtual ~CompilerError() = default;

  virtual std::string to_string() const = 0;
};

class ArgumentCountError : public CompilerError {
private:
  size_t expected;
  size_t actual;

  ArgumentCountError(size_t expected, size_t actual) : expected(expected), actual(actual){};

public:
  static std::optional<ArgumentCountError> check(llvm::Function *function, size_t argument_count);

  std::string to_string() const final;
};

class UncallableError : public CompilerError {
private:
  const llvm::Value *callee;

  UncallableError(const llvm::Value *callee) : callee(callee) {}

public:
  static std::optional<UncallableError> check(const llvm::Value *callee);

  std::string to_string() const final;
};

class NonhomogenousListError : public CompilerError {
private:
  const llvm::Type *expected;
  const llvm::Type *actual;

  NonhomogenousListError(const llvm::Type *expected, const llvm::Type *actual)
      : expected(expected), actual(actual){};

public:
  static std::optional<NonhomogenousListError> check(const llvm::Type *expected,
                                                     const llvm::Type *actual);

  std::string to_string() const final;
};

#endif
