#include <iostream>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "Compiler.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab {

void Compiler::save_module(const std::string &path) const {
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

void Compiler::compile(std::unique_ptr<Parser::RootNode> root) {
  this->load_globals();
  this->start_main();

  root->compile(*this);

  this->stop_main();
  this->save_module("./out.ll");
}

[[noreturn]] void Compiler::error(const std::string &message) const {
  // TODO: better tracing
  std::cerr << "compiler-error: " << message << std::endl;

  exit(1);
}

llvm::GlobalVariable *Compiler::create_global(const std::string &name, llvm::Constant *init,
                                              llvm::Type *type) {
  // this->module takes ownership of this object
  return new llvm::GlobalVariable(this->module, type, false, llvm::GlobalValue::ExternalLinkage,
                                  init, name);
}

llvm::AllocaInst *Compiler::create_local(const std::string &name, llvm::Constant *init,
                                         llvm::Type *type) {
  llvm::AllocaInst *local = this->builder.CreateAlloca(type, nullptr, name);
  this->builder.CreateStore(init, local);

  return local;
}

std::optional<llvm::LoadInst *> Compiler::get_global(const std::string &name) {
  llvm::GlobalVariable *global = this->module.getGlobalVariable(name);
  if (global == nullptr)
    return std::nullopt;
  else
    return this->builder.CreateLoad(global->getValueType(), global);
}

std::optional<llvm::LoadInst *> Compiler::get_local(const std::string &name) {
  llvm::Function *function_context = this->builder.GetInsertBlock()->getParent();
  for (llvm::BasicBlock &basic_block : *function_context)
    for (llvm::Instruction &instruction : basic_block) {
      llvm::AllocaInst *local = llvm::dyn_cast<llvm::AllocaInst>(&instruction);
      if (local != nullptr && local->getName() == name)
        return this->builder.CreateLoad(local->getAllocatedType(), local);
    }

  return std::nullopt;
}

std::optional<llvm::Function *> Compiler::get_function(const std::string &name) {
  if (llvm::Function *function = this->module.getFunction(name))
    return function;
  else
    return std::nullopt;
}

std::optional<llvm::Value *> Compiler::get_value(const std::string &name) {
  // Order of lookup is:
  // 1 local (stack allocated in scope)
  // 2 function
  // 3 global

  if (std::optional<llvm::Value *> local = this->get_local(name))
    return local;
  else if (std::optional<llvm::Value *> function = this->get_function(name))
    return function;
  else if (std::optional<llvm::Value *> global = this->get_global(name))
    return global;
  else
    return std::nullopt;
}

} // namespace Kebab
