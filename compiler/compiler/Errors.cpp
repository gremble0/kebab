#include <cassert>
#include <format>
#include <optional>
#include <string>

#include "compiler/Errors.hpp"
#include "compiler/Scope.hpp"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"

// TODO: type_to_string(llvm::Type *) helper method in superclass

std::optional<ArgumentCountError> ArgumentCountError::check(llvm::Function *function,
                                                            size_t argument_count) {
  size_t function_arg_size = function->arg_size();

  if (function->isVarArg()) {
    if (argument_count < function_arg_size)
      // currently all variadic functions are extern meaning they dont take a closure so no -1
      return ArgumentCountError(function_arg_size, argument_count);
  } else {
    if (function_arg_size != argument_count)
      // -1 for closure (user shouldn't see that this is an argument to their function)
      return ArgumentCountError(function_arg_size - 1, argument_count - 1);
  }

  return std::nullopt;
}

std::string ArgumentCountError::to_string() const {
  return std::format("argument-count-error: called function expects {} arguments, but was given {}",
                     std::to_string(this->expected), std::to_string(this->actual));
}

std::optional<UncallableError> UncallableError::check(const llvm::Value *callee) {
  if (llvm::isa<llvm::Function>(callee))
    return std::nullopt;
  else
    return UncallableError(callee);
}

std::string UncallableError::to_string() const {
  std::string callee_type_string;
  llvm::raw_string_ostream callee_type_stream(callee_type_string);
  this->callee->getType()->print(callee_type_stream);

  return std::format("uncallable-error: value of type '{}' is not callable", callee_type_string);
}

std::optional<UnsubscriptableError> UnsubscriptableError::check(const llvm::Value *subscriptee) {
  // TODO: this is kinda meh, but it works
  const llvm::LoadInst *load = llvm::dyn_cast<llvm::LoadInst>(subscriptee);
  if (load == nullptr)
    return UnsubscriptableError(subscriptee);

  const llvm::Value *pointee = load->getPointerOperand();
  const llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(pointee);
  if (alloca == nullptr)
    return UnsubscriptableError(subscriptee);

  if (!alloca->isArrayAllocation())
    return UnsubscriptableError(subscriptee);

  return std::nullopt;
}

std::string UnsubscriptableError::to_string() const {
  std::string subscriptee_type_string;
  llvm::raw_string_ostream subscriptee_type_stream(subscriptee_type_string);
  subscriptee->getType()->print(subscriptee_type_stream);

  return std::format("unsubscriptable-error: variable with type '{}' cannot be subscripted with []",
                     subscriptee_type_string);
}

std::optional<NonhomogenousListError> NonhomogenousListError::check(const llvm::Type *expected,
                                                                    const llvm::Type *actual) {
  if (expected == actual)
    return std::nullopt;
  else
    return NonhomogenousListError(expected, actual);
}

std::string NonhomogenousListError::to_string() const {
  std::string expected_string;
  llvm::raw_string_ostream expected_stream(expected_string);
  this->expected->print(expected_stream);

  std::string actual_string;
  llvm::raw_string_ostream actual_stream(actual_string);
  this->actual->print(actual_stream);

  return std::format("nonhomogenous-list-error: all elements in a list must be of the same type. "
                     "first element in list of type '{}' is different from element of type '{}'",
                     expected_string, actual_string);
}

std::optional<ImmutableAssignmentError>
ImmutableAssignmentError::check(const Scope &scope, const std::string &assignee) {
  std::optional<Scope::Binding> maybe_binding = scope.lookup(assignee);
  assert(maybe_binding.has_value() &&
         "assignments should not reach immutability check if name is not in scope");

  if (maybe_binding->is_mutable)
    return std::nullopt;
  else
    return ImmutableAssignmentError(assignee);
}

std::string ImmutableAssignmentError::to_string() const {
  return std::format("immutable-assignment-error: cannot assign to immutable value '{}'",
                     this->assignee);
}

std::optional<RedefinitionError> RedefinitionError::check(const Scope &scope,
                                                          const std::string &assignee) {
  std::optional<Scope::Binding> maybe_binding = scope.lookup(assignee);
  if (maybe_binding.has_value())
    return RedefinitionError(assignee);
  else
    return std::nullopt;
}

std::string RedefinitionError::to_string() const {
  return std::format("reassignment-error: cannot redefine already defined value '{}'",
                     this->assignee);
}
