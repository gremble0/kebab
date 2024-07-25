#include <memory>
#include <optional>
#include <unordered_map>

#include "llvm/IR/Value.h"

class Scope {
private:
  std::unordered_map<std::string, llvm::Value *> map;
  std::optional<std::shared_ptr<Scope>> parent;

public:
  Scope() = default;
  Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  std::shared_ptr<Scope> parent_or(std::shared_ptr<Scope> alternative) const {
    return this->parent.value_or(alternative);
  }

  std::optional<llvm::Value *> lookup(const std::string &key);
  void put(const std::string &key, llvm::Value *value);
};
