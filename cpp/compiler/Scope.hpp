#include <memory>
#include <optional>
#include <unordered_map>

#include "llvm/IR/Value.h"

class Scope {
private:
  std::optional<std::shared_ptr<Scope>> parent;
  std::unordered_map<std::string, llvm::Value *> map;

public:
  Scope() = default;
  Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  llvm::Value *operator[](const std::string &key);
};
