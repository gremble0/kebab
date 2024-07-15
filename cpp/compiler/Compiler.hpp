#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <memory>
#include <optional>
#include <string>

// Disable unused parameter warnings for llvm headers
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#pragma clang diagnostic pop

namespace Kebab {
// Can't unclude RootNode due to circular imports so have to declare independently
namespace Parser {
class RootNode;
}

class Compiler {
private:
  void save_module(const std::string &path) const;

  void load_printf();
  void load_globals();

  void start_main();
  void stop_main();

  std::optional<llvm::Value *> get_global(const std::string &name) const;
  std::optional<llvm::Value *> get_local(const std::string &name) const;
  std::optional<llvm::Function *> get_function(const std::string &name) const;

public:
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;

  Compiler() : context(), module("kebab", context), builder(context) {}

  void compile(std::unique_ptr<Parser::RootNode> root);
  [[noreturn]] void error(const std::string &message) const;

  // TODO: take type as param and compare with type of initializer
  llvm::GlobalVariable *create_global(const std::string &name, llvm::Constant *init,
                                      llvm::Type *type);
  llvm::AllocaInst *create_local(const std::string &name, llvm::Constant *init, llvm::Type *type);

  std::optional<llvm::Value *> get_variable(const std::string &name) const;
};

} // namespace Kebab

#endif
