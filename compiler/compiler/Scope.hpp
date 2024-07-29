#include <memory>
#include <optional>
#include <unordered_map>

#include "llvm/IR/Value.h"

class Scope {
private:
  struct Binding {
    bool is_mutable;
    llvm::Value *value;
    llvm::Type *type;
  };

  std::unordered_map<std::string, Binding> map;
  std::optional<std::shared_ptr<Scope>> parent;

public:
  Scope() = default;
  explicit Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  std::shared_ptr<Scope> parent_or(std::shared_ptr<Scope> alternative) const {
    return this->parent.value_or(alternative);
  }

  std::optional<llvm::Value *> lookup(const std::string &key);
  bool put(const std::string &key, llvm::Value *value, llvm::Type *type, bool is_mutable = false);
};
