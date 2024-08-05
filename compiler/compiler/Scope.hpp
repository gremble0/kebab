#ifndef KEBAB_SCOPE_HPP
#define KEBAB_SCOPE_HPP

#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include "llvm/IR/Value.h"
#pragma clang diagnostic pop

class Scope {

public:
  struct Binding {
    bool is_mutable;
    llvm::Value *value;
    llvm::Type *type;
  };

  Scope() = default;
  explicit Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  std::shared_ptr<Scope> parent_or(std::shared_ptr<Scope> alternative) const {
    return this->parent.value_or(alternative);
  }

  std::optional<Binding> lookup(const std::string &key) const;
  void put(const std::string &key, llvm::Value *value, llvm::Type *type, bool is_mutable = false);

  std::vector<std::pair<const std::string &, Binding>> bindings() const;

private:
  std::unordered_map<std::string, Binding> map;
  std::optional<std::shared_ptr<Scope>> parent;
};

#endif
