#include <memory>
#include <unordered_map>

#include "llvm/IR/Value.h"

class Scope {
private:
  std::shared_ptr<Scope> parent;
  std::unordered_map<std::string, llvm::Value *> map;

public:
  Scope();
  Scope(std::shared_ptr<Scope> parent) : parent(parent) {}

  llvm::Value *operator[](const std::string &key);
};
