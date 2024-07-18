#include <iostream>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "Compiler.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab {

void Compiler::save_module(const std::string &path) const {
  std::error_code error_code;
  llvm::raw_fd_stream fd(path, error_code);
  this->module.print(fd, nullptr);
}

void Compiler::load_printf() {
  // printfs real return type is int32 however we just say that it is int64 to make it easier to
  // deal with since the rest of the language uses 64 bit integers. This may cause some issues
  // however I have not encountered any yet so I will keep it as is until I need to change it
  llvm::IntegerType *return_type = this->builder.getInt64Ty();
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
  // TODO: type checking here maybe
  // this->module takes ownership of this object
  return new llvm::GlobalVariable(this->module, type, false, llvm::GlobalValue::ExternalLinkage,
                                  init, name);
}

llvm::AllocaInst *Compiler::create_local(const std::string &name, llvm::Constant *init,
                                         llvm::Type *type) {
  // Derive the alignment from the type
  const llvm::DataLayout &dataLayout = this->module.getDataLayout();
  llvm::Align alignment = dataLayout.getPrefTypeAlign(type);

  llvm::AllocaInst *local = this->builder.CreateAlloca(type, nullptr, name);
  local->setAlignment(llvm::Align(alignment));

  llvm::StoreInst *store = this->builder.CreateStore(init, local);
  store->setAlignment(llvm::Align(alignment));

  return local;
}

llvm::Value *Compiler::create_add(llvm::Value *lhs, llvm::Value *rhs) {
  // TODO: general function for type checking maybe, also check rhs somehow - currently we could end
  // up doing pointer arithmetic on accident
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFAdd(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateAdd(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for addition");
}

llvm::Value *Compiler::create_sub(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFSub(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateSub(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for subtraction");
}

llvm::Value *Compiler::create_mul(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFMul(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateMul(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for multiplication");
}

llvm::Value *Compiler::create_div(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFDiv(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateSDiv(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for division");
}

llvm::Value *Compiler::create_lt(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULT(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpULT(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for < operator");
}

llvm::Value *Compiler::create_le(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULE(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpULE(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for <= operator");
}

llvm::Value *Compiler::create_eq(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUEQ(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpEQ(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for == operator");
}

llvm::Value *Compiler::create_neq(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUNE(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpNE(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for ~= operator");
}

llvm::Value *Compiler::create_gt(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGT(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpUGT(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for > operator");
}

llvm::Value *Compiler::create_ge(llvm::Value *lhs, llvm::Value *rhs) {
  llvm::Type *lhs_type = lhs->getType();
  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGE(lhs, rhs);
  else if (lhs_type->isIntegerTy())
    return this->builder.CreateICmpUGE(lhs, rhs);
  else
    this->error("unsupported type `TODO: print type` for >= operator");
}

llvm::BasicBlock *Compiler::create_basic_block(llvm::Function *parent, const std::string &name) {
  return llvm::BasicBlock::Create(this->context, name, parent);
}
llvm::BranchInst *Compiler::create_branch(llvm::BasicBlock *destination) {
  return this->builder.CreateBr(destination);
}

llvm::BranchInst *Compiler::create_cond_branch(llvm::Value *condition,
                                               llvm::BasicBlock *true_destination,
                                               llvm::BasicBlock *false_destination) {

  return this->builder.CreateCondBr(condition, true_destination, false_destination);
}

llvm::PHINode *
Compiler::create_phi(llvm::Type *type,
                     std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> incoming) {
  llvm::PHINode *phi = this->builder.CreatePHI(type, incoming.size());
  for (const std::pair<llvm::Value *, llvm::BasicBlock *> &i : incoming)
    phi->addIncoming(i.first, i.second);

  return phi;
}

std::optional<llvm::Value *> Compiler::get_global(const std::string &name) {
  // Could use ValueSymbolTable, but that is what getGlobalVariable does anyways
  llvm::GlobalVariable *global = this->module.getGlobalVariable(name);
  if (global == nullptr)
    return std::nullopt;
  else
    return this->builder.CreateLoad(global->getValueType(), global);
}

std::optional<llvm::Value *> Compiler::get_local(const std::string &name) {
  // This will only search from top to bottom meaning if there are shadowed variable names or
  // variables with same names in different branches it will only return the first occurance since
  // they will compile down to having different names (e.g. local, local1, local2, etc.). This could
  // maybe be fixed by making the parser also add these suffixes to duplicate variables
  llvm::Function *current_function = this->get_current_function();
  llvm::ValueSymbolTable *symbolTable = current_function->getValueSymbolTable();

  llvm::Value *value = symbolTable->lookup(name);

  if (value == nullptr)
    return std::nullopt;

  if (llvm::AllocaInst *local = llvm::dyn_cast<llvm::AllocaInst>(value))
    return this->builder.CreateLoad(local->getAllocatedType(), local);
  else if (llvm::Argument *arg = llvm::dyn_cast<llvm::Argument>(value))
    return arg;

  // Unknown value - cant generate load instruction so return nullopt
  return std::nullopt;
}

std::optional<llvm::Function *> Compiler::get_function(const std::string &name) const {
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

  std::optional<llvm::Value *> local = this->get_local(name);
  if (local.has_value())
    return local;

  std::optional<llvm::Value *> function = this->get_function(name);
  if (function.has_value())
    return function;

  std::optional<llvm::Value *> global = this->get_global(name);
  if (global.has_value())
    return global;

  return std::nullopt;
}

llvm::Function *Compiler::get_current_function() const {
  return this->builder.GetInsertBlock()->getParent();
}

} // namespace Kebab
