#include <optional>
#include <set>

#include "compiler/Scope.hpp"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"

std::optional<Scope::Binding> Scope::lookup(const std::string &key) const {
  // Either:
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

std::vector<std::pair<const std::string &, Scope::Binding>> Scope::bindings() const {
  std::vector<std::pair<const std::string &, Binding>> local_bindings;

  for (const auto &[key, binding] : this->map)
    local_bindings.push_back({key, binding});

  // The most local bindings shadow any potential parent bindings with the same name
  std::set<std::string> seen;
  for (const auto &[key, _] : local_bindings)
    seen.insert(key);

  if (this->parent.has_value()) {
    auto parent_bindings = this->parent.value()->bindings();
    for (const auto &[key, binding] : parent_bindings) {
      // Dont add shadowed bindings or values that dont make sense to load (e.g. functions)
      if (seen.find(key) != seen.end() || !binding.type->isSized())
        continue;

      local_bindings.push_back({key, binding});
      seen.insert(key);
    }
  }

  return local_bindings;
}
