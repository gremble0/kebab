#include <memory>

#include "parser/Parser.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class Compiler {
private:
  // TODO: why pointers?
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::Module> module;
  std::unique_ptr<llvm::IRBuilder<>> builder;

  void save_module(const std::string &path);

public:
  Compiler()
      : context(std::make_unique<llvm::LLVMContext>()),
        module(std::make_unique<llvm::Module>("kebab", *context)),
        builder(std::make_unique<llvm::IRBuilder<>>(*context)) {}

  void compile(AstNode root);
};
