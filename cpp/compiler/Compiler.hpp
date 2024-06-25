#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "parser/Parser.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

class Compiler {
private:
  // TODO: why pointers?
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;

  void save_module(const std::string &path);

  void load_printf();
  void load_globals();

  void start_main();
  void stop_main();

public:
  Compiler()
      : context(llvm::LLVMContext()), module(llvm::Module("kebab", context)),
        builder(llvm::IRBuilder<>(context)) {}

  void compile(AstNode *root);
};

#endif
