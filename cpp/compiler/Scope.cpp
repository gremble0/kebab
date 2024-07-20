#include "Scope.hpp"
#include "llvm/IR/Value.h"

llvm::Value *Scope::operator[](const std::string &key) {
  llvm::Value *value = this->map[key];
  if (value == nullptr && this->parent.has_value())
    return (*this->parent->get())[key];
  else
    return value;
}
