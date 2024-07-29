#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <vector>

#include "compiler/Compiler.hpp"
#include "parser/Constructor.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab {

void Compiler::compile(std::unique_ptr<Parser::RootNode> root, const std::string &output_path) {
  this->load_globals();
  root->compile(*this);
  this->save_module(output_path);
}

// ConstantArray?
llvm::LoadInst *Compiler::create_list(const std::vector<llvm::Value *> &list, llvm::Type *type) {
  llvm::AllocaInst *alloca = this->create_alloca("", list, type);

  llvm::Type *list_type = alloca->getAllocatedType();
  llvm::LoadInst *load = this->builder.CreateLoad(list_type, alloca);
  load->setAlignment(this->get_alignment(list_type));

  return load;
}

void Compiler::save_module(const std::string &path) const {
  std::error_code error_code;
  llvm::raw_fd_stream fd(path, error_code);
  this->mod.print(fd, nullptr);
}

void Compiler::load_printf() {
  // `printfs` real return type is int32 however we just say that it is int64 to make it easier to
  // deal with since the rest of the language uses 64 bit integers. This may cause some issues
  // however I have not encountered any yet so I will keep it as is until I need to change it
  //
  // The signature of this function is `i64 printf(i8 *, ...)`
  llvm::IntegerType *return_type = this->get_int_type();
  llvm::PointerType *format_type = this->get_string_type();
  llvm::FunctionType *prototype = llvm::FunctionType::get(return_type, format_type, true);

  this->declare_function(prototype, "printf");
}

void Compiler::load_globals() { this->load_printf(); }

llvm::Value *Compiler::int_to_float(llvm::Value *i) {
  return this->builder.CreateCast(llvm::Instruction::SIToFP, i, this->get_float_type());
}

llvm::Function *Compiler::declare_function(llvm::FunctionType *type, const std::string &name) {
  llvm::Function *function =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->mod);

  this->current_scope->put(name, function, function->getFunctionType());

  return function;
}

llvm::Function *Compiler::define_function(
    llvm::FunctionType *type, const std::string &name, const Parser::Constructor &body,
    const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters) {
  this->start_scope();

  llvm::Function *function =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->mod);

  // Set parameter names
  for (size_t i = 0, size = parameters.size(); i < size; ++i) {
    llvm::Argument *argument = function->getArg(i);
    argument->setName(parameters[i]->name);
    // TODO: mutability for parameters maybe with mut keyword for mutable params. This would have to
    // be changed in parser as well
    this->current_scope->put(parameters[i]->name, argument, argument->getType());
  }

  // Make entry for new function and save the current insert block so we can return to it after
  // we're done compiling the current function
  llvm::BasicBlock *entry = this->create_basic_block(function, "entry");
  llvm::BasicBlock *previous_block = this->builder.GetInsertBlock();

  this->set_insert_point(entry);
  this->builder.CreateRet(body.compile(*this));
  this->set_insert_point(previous_block);

  this->end_scope();
  // Previous scope now has this function in scope - this allows for first order functions since no
  // parent scopes can't see this binding
  this->current_scope->put(name, function, function->getFunctionType());

  return function;
}

llvm::Align Compiler::get_alignment(llvm::Type *type) const {
  const llvm::DataLayout &layout = this->mod.getDataLayout();
  return layout.getPrefTypeAlign(type);
}

llvm::AllocaInst *Compiler::create_alloca(const std::string &name, llvm::Constant *init,
                                          llvm::Type *type) {
  llvm::Align alignment = this->get_alignment(type);
  llvm::AllocaInst *local = this->builder.CreateAlloca(type, nullptr, name);
  local->setAlignment(alignment);

  llvm::StoreInst *store = this->builder.CreateStore(init, local);
  store->setAlignment(alignment);

  return local;
}

llvm::AllocaInst *Compiler::create_alloca(const std::string &name,
                                          const std::vector<llvm::Value *> &init,
                                          llvm::Type *type) {
  llvm::ArrayType *list_type = llvm::ArrayType::get(type, init.size());
  llvm::AllocaInst *local =
      this->builder.CreateAlloca(list_type, this->create_int(init.size()), name);
  llvm::Align alignment = this->get_alignment(list_type);
  local->setAlignment(alignment);

  llvm::ConstantInt *index0 = this->create_int(0);

  for (size_t i = 0, size = init.size(); i < size; ++i) {
    llvm::Value *offset =
        this->builder.CreateInBoundsGEP(list_type, local, {index0, this->create_int(i)});
    this->builder.CreateStore(init[i], offset);
  }

  return local;
}

std::optional<llvm::AllocaInst *>
Compiler::create_immutable(const std::string &name, llvm::Constant *init, llvm::Type *type) {
  llvm::AllocaInst *local = this->create_alloca(name, init, type);
  llvm::LoadInst *load = this->builder.CreateLoad(type, local);
  load->setAlignment(this->get_alignment(type));

  if (this->current_scope->put(name, load, type))
    return local;
  else
    return std::nullopt;
}

std::optional<llvm::AllocaInst *> Compiler::create_mutable(const std::string &name,
                                                           llvm::Constant *init, llvm::Type *type) {
  llvm::AllocaInst *local = this->create_alloca(name, init, type);
  llvm::LoadInst *load = this->builder.CreateLoad(type, local);
  load->setAlignment(this->get_alignment(type));

  if (this->current_scope->put(name, load, type, true))
    return local;
  else
    return std::nullopt;
}

std::optional<llvm::Value *> Compiler::create_neg(llvm::Value *v) {
  // unary - is only allowed for numbers
  llvm::Type *v_type = v->getType();
  if (v_type->isIntegerTy(64))
    return this->builder.CreateNeg(v);
  else if (v_type->isDoubleTy())
    return this->builder.CreateFNeg(v);
  else
    return std::nullopt;
}

// returns nullopt if out of bounds
std::optional<llvm::Value *> Compiler::create_subscription(llvm::AllocaInst *array,
                                                           llvm::Value *offset) {
  // TODO: boundscheck `getArraySize()`
  return this->builder.CreateGEP(array->getAllocatedType(), array, {this->create_int(0), offset});
}

std::optional<llvm::Value *> Compiler::create_add(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  // Only floats and ints can be added. We could raise error here but we don't have tracing
  // information so its better if the caller (AstNode) does it
  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    // If lhs is a float do straightforward floating point addition
    return this->builder.CreateFAdd(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    // If lhs is not a float but rhs is we need to cast lhs to a float before we can add
    return this->builder.CreateFAdd(this->int_to_float(lhs), rhs);
  else
    // If neither lhs nor rhs are floats they are both ints (verified by guard clause above)
    return this->builder.CreateAdd(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_sub(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFSub(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFSub(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateSub(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_mul(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFMul(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFMul(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateMul(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_div(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFDiv(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFDiv(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateSDiv(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_lt(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULT(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpULT(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpULT(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_le(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULE(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpULE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpULE(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_eq(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  // == is allowed for bools
  if (lhs_type->isIntegerTy(1) && rhs_type->isIntegerTy(1))
    return this->builder.CreateICmpEQ(lhs, rhs);

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUEQ(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpUEQ(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpEQ(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_neq(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if (lhs_type->isIntegerTy(1) && rhs_type->isIntegerTy(1))
    return this->builder.CreateICmpNE(lhs, rhs);

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUNE(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpUNE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpNE(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_gt(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGT(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGT(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpSGT(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_ge(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();

  if ((!lhs_type->isDoubleTy() && !lhs_type->isIntegerTy(64)) ||
      (!rhs_type->isDoubleTy() && !rhs_type->isIntegerTy(64)))
    return std::nullopt;

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGE(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpUGE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpSGE(lhs, rhs);
}

std::optional<llvm::Value *> Compiler::create_and(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  if (lhs_type->isIntegerTy(1) && rhs_type->isIntegerTy(1))
    return this->builder.CreateAnd(lhs, rhs);
  else
    return std::nullopt;
}

std::optional<llvm::Value *> Compiler::create_or(llvm::Value *lhs, llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  if (lhs_type->isIntegerTy(1) && rhs_type->isIntegerTy(1))
    return this->builder.CreateOr(lhs, rhs);
  else
    return std::nullopt;
}

std::optional<llvm::Value *> Compiler::create_not(llvm::Value *v) {
  if (v->getType()->isIntegerTy(1))
    return this->builder.CreateNot(v);
  else
    return std::nullopt;
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
                     const std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> &incoming) {
  llvm::PHINode *phi = this->builder.CreatePHI(type, incoming.size());
  for (const auto &[value, branch] : incoming)
    phi->addIncoming(value, branch);

  return phi;
}

} // namespace Kebab
