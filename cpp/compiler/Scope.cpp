#include <iostream>

#include "Scope.hpp"
#include "llvm/IR/Value.h"

llvm::Value *Scope::operator[](const std::string &key) {
  std::cout << "getting value: " << key << std::endl;
  llvm::Value *value = this->map[key];
  std::cout << "value: " << value << std::endl;
  return value;
}
