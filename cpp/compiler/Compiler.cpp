#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "Compiler.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab {
namespace Compiler {

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

void Compiler::compile(std::unique_ptr<Parser::RootNode> root) {
  this->load_globals();
  this->start_main();

  root->compile(*this);

  this->stop_main();
  this->save_module("./out.ll");
}

void Compiler::create_variable(llvm::Type *type, const std::string &name,
                               std::optional<llvm::Value *> init_value) {
  this->current_variable = this->builder.CreateAlloca(type, nullptr, name);
  if (init_value != std::nullopt)
    this->builder.CreateStore(init_value.value(), this->current_variable);
}

} // namespace Compiler
} // namespace Kebab
