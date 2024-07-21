#include <memory>
#include <optional>
#include <unordered_map>

#include "llvm/IR/Value.h"

class Scope {
private:
  std::unordered_map<std::string, llvm::Value *> map;

public:
  std::optional<std::shared_ptr<Scope>> parent;
  Scope() = default;
  Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  std::optional<llvm::Value *> lookup(const std::string &key);
  void put(const std::string &key, llvm::Value *value);
};
