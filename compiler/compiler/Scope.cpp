#include <optional>

#include "compiler/Scope.hpp"
#include "llvm/IR/Value.h"

std::optional<Scope::Binding> Scope::lookup(const std::string &key) {
  // Either
  // 1. we have found the value -> return it
  // 2. we have not found the value, but we have a parent -> look in parent
  // 3. we have not found the value and we dont have a parent -> return nullopt
  auto it = this->map.find(key);
  if (it != this->map.end())
    return it->second;
  else if (this->parent.has_value())
    return this->parent.value()->lookup(key);
  else
    return std::nullopt;
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

std::vector<std::pair<std::string, Scope::Binding>> Scope::bindings() {
  std::vector<std::pair<std::string, Binding>> local_bindings;

  for (const auto &[key, binding] : this->map)
    local_bindings.push_back({key, binding});

  // Only add parent values that are not already present in the more local scope
  // TODO: this could be optimized with something like a set
  if (this->parent.has_value()) {
    auto parent_bindings = this->parent.value()->bindings();
    for (const auto &[key, binding] : parent_bindings) {
      bool is_shadowed = false;
      for (const auto &[local_key, local_binding] : local_bindings)
        if (local_key == key)
          is_shadowed = true;

      if (!is_shadowed)
        local_bindings.push_back({key, binding});
    }
  }

  return local_bindings;
}
