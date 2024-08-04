#include <cassert>
#include <format>
#include <optional>
#include <string>

#include "compiler/Errors.hpp"
#include "compiler/Scope.hpp"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"

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
  std::string type_string = CompilerError::type_to_string(this->callee->getType());

  return std::format("uncallable-error: value of type '{}' is not callable", type_string);
}

std::optional<UnsubscriptableError> UnsubscriptableError::check(const llvm::Value *subscriptee) {
  if (subscriptee->getType()->isArrayTy())
    return std::nullopt;
  else
    return UnsubscriptableError(subscriptee);
}

std::string UnsubscriptableError::to_string() const {
  std::string type_string = CompilerError::type_to_string(this->subscriptee->getType());

  return std::format("unsubscriptable-error: variable with type '{}' cannot be subscripted with []",
                     type_string);
}

std::optional<IndexError> IndexError::check(const llvm::AllocaInst *subscriptee,
                                            const llvm::Value *index) {
  // TODO:
  return std::nullopt;
}

std::string IndexError::to_string() const {
  // TODO:
  return std::format("index-error: index out of bounds");
}

std::optional<NonhomogenousListError> NonhomogenousListError::check(const llvm::Type *expected,
                                                                    const llvm::Type *actual) {
  if (expected == actual)
    return std::nullopt;
  else
    return NonhomogenousListError(expected, actual);
}

std::string NonhomogenousListError::to_string() const {
  std::string expected_string = CompilerError::type_to_string(this->expected);
  std::string actual_string = CompilerError::type_to_string(this->actual);

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

std::optional<AssignNonExistingError> AssignNonExistingError::check(const Scope &scope,
                                                                    const std::string &assignee) {
  std::optional<Scope::Binding> maybe_binding = scope.lookup(assignee);
  if (maybe_binding.has_value())
    return std::nullopt;
  else
    return AssignNonExistingError(assignee);
}

std::string AssignNonExistingError::to_string() const {
  return std::format("assign-nonexisting-error: cannot assign to non existing value '{}'",
                     this->assignee);
}

std::optional<UnrecognizedTypeError>
UnrecognizedTypeError::check(const std::unordered_map<std::string, llvm::Type *> &known_types,
                             const std::string &type_name) {
  if (known_types.find(type_name) == known_types.end())
    return UnrecognizedTypeError(type_name);
  else
    return std::nullopt;
}

std::string UnrecognizedTypeError::to_string() const {
  return std::format("unrecognized-type-error: unrecognized type '{}'", this->type_name);
}

std::optional<NameError> NameError::check(const Scope &scope, const std::string &name) {
  if (scope.lookup(name).has_value())
    return std::nullopt;
  else
    return NameError(name);
}

std::string NameError::to_string() const {
  return std::format("name-error: undeclared identifier '{}'", this->name);
}
