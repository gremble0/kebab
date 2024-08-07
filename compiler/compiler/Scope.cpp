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

void Scope::put(const std::string &key, llvm::Value *value, llvm::Type *type, bool is_mutable) {
  if (this->map.contains(key))
    this->map[key].value = value;

  this->map[key] = Binding{is_mutable, value, type};
}

std::vector<std::pair<const std::string &, Scope::Binding>> Scope::bindings() const {
  std::vector<std::pair<const std::string &, Binding>> local_bindings;
  std::set<std::string> seen;

  // The most local bindings shadow any potential parent bindings with the same name
  for (const auto &[key, binding] : this->map)
    if (binding.type->isSized()) {
      local_bindings.push_back({key, binding});
      seen.insert(key);
    }

  if (this->parent.has_value()) {
    auto parent_bindings = this->parent.value()->bindings();
    for (const auto &[key, binding] : parent_bindings) {
      // Dont add shadowed bindings or values that dont make sense to load (e.g. functions)
      if (seen.contains(key) || !binding.type->isSized())
        continue;

      local_bindings.push_back({key, binding});
      seen.insert(key);
    }
  }

  return local_bindings;
}
