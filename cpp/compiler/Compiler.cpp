#include <system_error>
#include <vector>

#include "Compiler.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

void Compiler::save_module(const std::string &path) {
  std::error_code error_code;
  llvm::raw_fd_stream fd(path, error_code);
  this->module.print(fd, nullptr);
}

void Compiler::load_printf() {
  llvm::IntegerType *return_type = this->builder.getInt32Ty();
  llvm::PointerType *format_type = this->builder.getInt8Ty()->getPointerTo();
  llvm::FunctionType *function_type = llvm::FunctionType::get(return_type, format_type, true);

  this->module.getOrInsertFunction("printf", function_type);
}

void Compiler::load_globals() { this->load_printf(); }

void Compiler::start_main() {
  llvm::IntegerType *return_type = this->builder.getInt32Ty();
  llvm::Type *argc_type = this->builder.getInt32Ty();
  llvm::Type *argv_type = this->builder.getInt8Ty()->getPointerTo()->getPointerTo();
  std::vector<llvm::Type *> param_types = {argc_type, argv_type};

  llvm::FunctionType *prototype = llvm::FunctionType::get(return_type, param_types, false);
  llvm::Function *main =
      llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, "main", this->module);

  llvm::BasicBlock *body = llvm::BasicBlock::Create(this->context, "entry", main);
  this->builder.SetInsertPoint(body);
}

void Compiler::stop_main() {
  // Return 0 as a 32 bit integer
  llvm::Value *return_value = this->builder.getInt32(0);
  auto return_value_i32 =
      this->builder.CreateIntCast(return_value, this->builder.getInt32Ty(), true);

  this->builder.CreateRet(return_value_i32);
}

void Compiler::compile(AstNode *root) {
  this->load_globals();
  this->start_main();

  llvm::Function *printf_fn = this->module.getFunction("printf");
  std::vector<llvm::Value *> args = {this->builder.CreateGlobalStringPtr("Hello, World!\n")};
  this->builder.CreateCall(printf_fn, args);

  this->stop_main();
  this->save_module("./out.ll");
}