#include <optional>

#include "Scope.hpp"
#include "llvm/IR/Value.h"

std::optional<llvm::Value *> Scope::lookup(const std::string &key) {
  // Either
  // 1. we have found the value -> return it
  // 2. we have not found the value, but we have a parent -> look in parent
  // 3. we have not found the value and we dont have a parent -> return nullopt
  const Binding &binding = this->map[key];
  if (binding.value == nullptr) {
    if (this->parent.has_value())
      return this->parent->get()->lookup(key);
    else
      return std::nullopt;
  } else {
    return binding.value;
  }
}

/**
 * @return true if successful insertion, false if attempt to override immutable value caused
 * insertion to fail
 */
bool Scope::put(const std::string &key, llvm::Value *value, llvm::Type *type, bool is_mutable) {
  if (this->map.contains(key)) {
    if (!this->map[key].is_mutable)
      return false;
    else
      this->map[key].value = value;
  }

  this->map[key] = {is_mutable, value, type};
  return true;
}
