#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "Compiler.hpp"
#include "parser/Constructor.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/Argument.h"
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
  // `printfs` real return type is int32 however we just say that it is int64 to make it easier to
  // deal with since the rest of the language uses 64 bit integers. This may cause some issues
  // however I have not encountered any yet so I will keep it as is until I need to change it
  //
  // The signature of this function is `i64 printf(i8 *, ...)`
  llvm::IntegerType *return_type = this->builder.getInt64Ty();
  llvm::PointerType *format_type = this->builder.getInt8Ty()->getPointerTo();
  llvm::FunctionType *prototype = llvm::FunctionType::get(return_type, format_type, true);

  this->declare_function(prototype, "printf");
}

void Compiler::load_globals() { this->load_printf(); }

void Compiler::compile(std::unique_ptr<Parser::RootNode> root) {
  this->load_globals();
  root->compile(*this);
  this->save_module("./out.ll");
}

[[noreturn]] void Compiler::error(const std::string &message) const {
  // TODO: better tracing
  std::cerr << "compiler-error: " << message << std::endl;

  exit(1);
}

llvm::Function *Compiler::declare_function(llvm::FunctionType *type, const std::string &name) {
  llvm::Function *function =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->module);

  this->current_scope->put(name, function);

  return function;
}

llvm::Function *Compiler::define_function(
    llvm::FunctionType *type, const std::string &name,
    const std::unique_ptr<Parser::Constructor> &body,
    const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters) {
  // Make new scope for this function, then return to previous scope after its done compiling
  std::shared_ptr<Scope> previous_scope = this->current_scope;
  this->current_scope = std::make_shared<Scope>(this->current_scope);

  llvm::Function *function =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->module);

  // Set parameter names
  for (size_t i = 0, size = parameters.size(); i < size; ++i) {
    llvm::Argument *argument = function->getArg(i);
    argument->setName(parameters[i]->name);
    this->current_scope->put(parameters[i]->name, argument);
  }

  // Make entry for new function and save the current insert block so we can return to it after
  // we're done compiling the current function
  llvm::BasicBlock *entry = this->create_basic_block(function, "entry");
  llvm::BasicBlock *previous_block = this->builder.GetInsertBlock();

  this->builder.SetInsertPoint(entry);
  this->builder.CreateRet(body->compile(*this));
  this->builder.SetInsertPoint(previous_block);

  this->current_scope = previous_scope;
  // Previous scope now has this function in scope - this allows for first order functions since no
  // parent scopes can't see this binding
  this->current_scope->put(name, function);

  return function;
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

  this->current_scope->put(name, local);

  return local;
}

llvm::Value *Compiler::create_add(llvm::Value *lhs, llvm::Value *rhs) {
  // TODO: general function for type checking maybe, also check rhs somehow - currently we could
  // end up doing pointer arithmetic on accident
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

std::optional<llvm::Value *> Compiler::get_value(const std::string &name) {
  return this->current_scope->lookup(name);
}

llvm::Function *Compiler::get_current_function() const {
  return this->builder.GetInsertBlock()->getParent();
}

} // namespace Kebab
