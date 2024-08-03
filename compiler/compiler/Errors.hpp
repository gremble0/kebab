#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include <cstddef>
#include <optional>

#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"

class ArgumentCountError {
private:
  size_t expected;
  size_t actual;

  ArgumentCountError(size_t expected, size_t actual) : expected(expected), actual(actual){};

public:
  static std::optional<ArgumentCountError> check(llvm::Function *function, size_t argument_count);

  std::string to_string() const;
};

class UncallableError {
private:
  const llvm::Value *callee;

  UncallableError(const llvm::Value *callee) : callee(callee) {}

public:
  static std::optional<UncallableError> check(const llvm::Value *callee);

  std::string to_string() const;
};

class NonhomogenousListError {
private:
  const llvm::Type *expected;
  const llvm::Type *actual;

  NonhomogenousListError(const llvm::Type *expected, const llvm::Type *actual)
      : expected(expected), actual(actual){};

public:
  static std::optional<NonhomogenousListError> check(const llvm::Type *expected,
                                                     const llvm::Type *actual);

  std::string to_string() const;
};

#endif
