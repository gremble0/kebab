#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include <cstddef>
#include <optional>

#include "compiler/Scope.hpp"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"

class CompilerError {
protected:
  static std::string type_to_string(const llvm::Type *type) {
    std::string callee_type_string;
    llvm::raw_string_ostream callee_type_stream(callee_type_string);
    type->print(callee_type_stream);
    return callee_type_string;
  }

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

class UnsubscriptableError : public CompilerError {
private:
  const llvm::Value *subscriptee;

  UnsubscriptableError(const llvm::Value *subscriptee) : subscriptee(subscriptee) {}

public:
  static std::optional<UnsubscriptableError> check(const llvm::Value *subscriptee);

  std::string to_string() const final;
};

class IndexError : public CompilerError {
private:
  const llvm::AllocaInst *list;
  const llvm::Value *index;

  IndexError(const llvm::AllocaInst *subscriptee, const llvm::Value *index)
      : list(subscriptee), index(index) {}

public:
  static std::optional<IndexError> check(const llvm::AllocaInst *subscriptee,
                                         const llvm::Value *index);

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

class ImmutableAssignmentError : public CompilerError {
private:
  const std::string &assignee;

  ImmutableAssignmentError(const std::string &assignee) : assignee(assignee) {}

public:
  static std::optional<ImmutableAssignmentError> check(const Scope &scope,
                                                       const std::string &assignee);

  std::string to_string() const final;
};

class RedefinitionError : public CompilerError {
private:
  const std::string &assignee;

  RedefinitionError(const std::string &assignee) : assignee(assignee) {}

public:
  static std::optional<RedefinitionError> check(const Scope &scope, const std::string &assignee);

  std::string to_string() const final;
};

class AssignNonExistingError : public CompilerError {
private:
  const std::string &assignee;

  AssignNonExistingError(const std::string &assignee) : assignee(assignee) {}

public:
  static std::optional<AssignNonExistingError> check(const Scope &scope,
                                                     const std::string &assignee);

  std::string to_string() const final;
};

#endif
