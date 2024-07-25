#include <optional>

#include "Scope.hpp"
#include "llvm/IR/Value.h"

std::optional<llvm::Value *> Scope::lookup(const std::string &key) {
  // Either
  // 1. we have found the value -> return it
  // 2. we have not found the value, but we have a parent -> look in parent
  // 3. we have not found the value and we dont have a parent -> return nullopt
  llvm::Value *value = this->map[key];
  if (value == nullptr) {
    if (this->parent.has_value())
      return this->parent->get()->lookup(key);
    else
      return std::nullopt;
  } else {
    return value;
  }
}

void Scope::put(const std::string &key, llvm::Value *value) { this->map.try_emplace(key, value); }
