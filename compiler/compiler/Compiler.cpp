#include <cassert>
#include <cstdlib>
#include <memory>
#include <optional>
#include <string>
#include <system_error>
#include <variant>
#include <vector>

#include "compiler/Compiler.hpp"
#include "compiler/Errors.hpp"
#include "parser/Constructor.hpp"
#include "parser/RootNode.hpp"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab {

// TODO: user defined global variables

void Compiler::compile(std::unique_ptr<Parser::RootNode> root, const std::string &output_path) {
  this->declare_extern_functions();
  root->compile(*this);
  this->save_module(output_path);
}

llvm::Value *Compiler::create_list(const std::vector<llvm::Value *> &list, llvm::Type *type) {
  // Calculate total size in bytes
  llvm::ConstantInt *list_size =
      this->create_int(list.size() * (type->getPrimitiveSizeInBits() / 8));

  // Allocate memory
  std::vector<llvm::Value *> malloc_args = {list_size};
  llvm::CallInst *alloc =
      std::get<llvm::CallInst *>(this->create_call(this->mod.getFunction("malloc"), malloc_args));
  llvm::Value *typed_alloc = this->builder.CreateBitCast(alloc, type->getPointerTo());

  // Fill list allocation with initializers
  for (size_t i = 0; i < list.size(); ++i) {
    llvm::Value *elementPtr = this->builder.CreateGEP(type, typed_alloc, this->create_int(i));
    this->create_store(list[i], elementPtr);
  }

  // Store information about list for later use (this information is not stored in the value itself
  // so it will get lost if we dont store it somewhere)
  this->list_infos[typed_alloc] = {type, list_size};
  return typed_alloc;
}

void Compiler::save_module(const std::string &path) const {
  std::error_code error_code;
  llvm::raw_fd_stream fd(path, error_code);
  this->mod.print(fd, nullptr);
}

void Compiler::declare_printf() {
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

void Compiler::declare_malloc() {
  // The signature of this function is `void *malloc(u32)` - but again, we just say that its
  // parameter is i64 for ease of use
  llvm::PointerType *return_type = this->get_void_type()->getPointerTo();
  llvm::IntegerType *size_type = this->get_int_type();
  llvm::FunctionType *prototype = llvm::FunctionType::get(return_type, size_type, false);

  this->declare_function(prototype, "malloc");
}

void Compiler::declare_extern_functions() {
  this->declare_printf();
  this->declare_malloc();
}

std::variant<llvm::Type *, UnrecognizedTypeError>
Compiler::get_primitive_type(const std::string &type_name) const {
  if (auto error = UnrecognizedTypeError::check(this->primitive_types, type_name);
      error.has_value())
    return error.value();

  auto it = this->primitive_types.find(type_name);
  // Should always be valid since we check if it doesnt exist above
  return it->second;
}

llvm::Value *Compiler::int_to_float(llvm::Value *i) {
  return this->builder.CreateCast(llvm::Instruction::SIToFP, i, this->get_float_type());
}

llvm::Function *Compiler::declare_function(llvm::FunctionType *type, const std::string &name) {
  llvm::Function *function =
      llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, this->mod);

  this->current_scope->put(name, function, type);

  return function;
}

void Compiler::load_arguments(
    const llvm::Function *function,
    const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters) {
  // Load and add fields of closure into scope
  // unsigned int because type is required by CreateExtractValue(), bindings.size - parameters.size
  // since bindings are expanded by parameters and we dont want to add these in the closure
  llvm::Argument *closure_arg = function->getArg(function->arg_size() - 1);
  closure_arg->setName("closure-env");

  std::vector<std::pair<const std::string &, Scope::Binding>> bindings =
      this->current_scope->bindings();

  for (unsigned int i = 0, size = bindings.size(); i < size; ++i) {
    const auto &[name, binding] = bindings[i];
    llvm::Value *field = this->builder.CreateExtractValue(closure_arg, {i}, "closure-env:" + name);
    llvm::AllocaInst *field_pointer = this->create_alloca("closure-env:" + name + "-ptr", field,
                                                          field->getType()->getPointerTo());
    llvm::LoadInst *field_loaded = this->create_load(field_pointer->getType(), field_pointer);

    this->current_scope->put(name, field_loaded, binding.type, binding.is_mutable);
  }

  // Set parameter names and bring parameters into scope of function
  for (size_t i = 0, size = parameters.size(); i < size; ++i) {
    llvm::Argument *argument = function->getArg(i);
    argument->setName(parameters[i]->name);

    llvm::AllocaInst *argument_alloca =
        this->create_alloca("arg:" + parameters[i]->name, argument, argument->getType());

    // TODO: mutability for parameters maybe with mut keyword for mutable params. This would have to
    // be changed in parser as well. For now just make all parameters const
    this->current_scope->put(parameters[i]->name, argument_alloca, argument->getType());
  }
}

llvm::Function *Compiler::define_function(
    const llvm::FunctionType *function_type, const std::string &name,
    const Parser::Constructor &body,
    const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters) {
  this->start_scope();

  llvm::StructType *closure_type = this->create_closure_type();
  llvm::FunctionType *function_type_with_closure = this->add_parameter(function_type, closure_type);

  llvm::Function *function = llvm::Function::Create(
      function_type_with_closure, llvm::Function::ExternalLinkage, name, this->mod);

  // Make entry for new function and save the current insert block so we can return to it after
  // we're done compiling the current function
  llvm::BasicBlock *entry = this->create_basic_block(function, "entry");
  llvm::BasicBlock *previous_block = this->builder.GetInsertBlock();

  // Codegen for the body of the function
  this->set_insert_point(entry);
  this->load_arguments(function, parameters);
  // For recursion the function needs to be defined within its own scope
  this->current_scope->put(name, function, function->getFunctionType());
  this->builder.CreateRet(body.compile(*this));
  this->set_insert_point(previous_block);

  this->end_scope();
  this->current_scope->put(name, function, function->getFunctionType());

  return function;
}

llvm::Align Compiler::get_alignment(llvm::Type *type) const {
  const llvm::DataLayout &layout = this->mod.getDataLayout();
  return layout.getPrefTypeAlign(type);
}

llvm::StructType *Compiler::create_closure_type() {
  std::vector<llvm::Type *> types;
  for (const auto &bindings = this->current_scope->bindings();
       const auto &[key, binding] : bindings)
    types.push_back(binding.type->getPointerTo());

  auto closure_type = llvm::StructType::get(this->context, types);
  closure_type->setName("closure-env");

  return closure_type;
}

llvm::FunctionType *Compiler::add_parameter(const llvm::FunctionType *type, llvm::Type *parameter) {
  std::vector<llvm::Type *> param_types = type->params();
  param_types.push_back(parameter);

  return llvm::FunctionType::get(type->getReturnType(), param_types, type->isVarArg());
}

llvm::Value *Compiler::create_closure_argument(llvm::StructType *closure_type) {
  llvm::Value *closure_argument = llvm::UndefValue::get(closure_type);

  std::vector<std::pair<const std::string &, Scope::Binding>> bindings =
      this->current_scope->bindings();
  for (unsigned int i = 0, num_elements = closure_type->getNumElements(); i < num_elements; ++i) {
    llvm::Value *field_value = bindings[i].second.value;
    closure_argument = this->builder.CreateInsertValue(closure_argument, field_value, {i});
  }
  closure_argument->setName("closure-arg");

  return closure_argument;
}

llvm::AllocaInst *Compiler::create_alloca(const std::string &name, llvm::Value *init,
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

llvm::LoadInst *Compiler::create_load(llvm::Type *type, llvm::Value *value) {
  llvm::LoadInst *load = this->builder.CreateLoad(type, value);
  load->setAlignment(this->get_alignment(type));

  return load;
}

llvm::StoreInst *Compiler::create_store(llvm::Value *init, llvm::Value *dest) {
  llvm::StoreInst *store = this->builder.CreateStore(init, dest);
  store->setAlignment(this->get_alignment(init->getType()));

  return store;
}

std::variant<llvm::AllocaInst *, RedefinitionError>
Compiler::create_definition(const std::string &name, llvm::Value *init, bool is_mutable) {
  if (auto error = RedefinitionError::check(*this->current_scope, name); error.has_value())
    return error.value();

  llvm::AllocaInst *local = this->create_alloca(name, init, init->getType());
  this->current_scope->put(name, local, init->getType(), is_mutable);

  // If we're creating a pointer to a list, copy the list info as well
  if (this->list_infos.contains(init))
    this->list_infos[local] = this->list_infos[init];

  return local;
}

std::variant<llvm::Value *, ImmutableAssignmentError, AssignNonExistingError, TypeError>
Compiler::create_assignment(const std::string &name, llvm::Value *init) {
  if (auto error = AssignNonExistingError::check(*this->current_scope, name); error.has_value())
    return error.value();

  if (auto error = ImmutableAssignmentError::check(*this->current_scope, name); error.has_value())
    return error.value();

  auto existing = this->current_scope->lookup(name);
  assert(existing.has_value() && "lookup failure should be caught by previous error checking");
  assert(existing->is_mutable &&
         "assignment to immutable should be caught by previous error checking");
  assert(existing->value->getType()->isPointerTy() &&
         "should be unreachable for non pointer values");

  if (auto error = TypeError::check(existing->type, init->getType()); error.has_value())
    return error.value();

  this->create_store(init, existing->value);

  return existing->value;
}

std::variant<llvm::Value *, UnaryOperatorError> Compiler::create_neg(llvm::Value *v) {
  const llvm::Type *v_type = v->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if (v_type == int_type)
    return this->builder.CreateNeg(v);
  else if (v_type == float_type)
    return this->builder.CreateFNeg(v);
  else
    return UnaryOperatorError(v_type, "-");
}

std::variant<llvm::Value *, UnaryOperatorError> Compiler::create_not(llvm::Value *v) {
  const llvm::Type *v_type = v->getType();
  const llvm::Type *bool_type = this->get_bool_type();

  if (v_type == bool_type)
    return this->builder.CreateNot(v);
  else
    return UnaryOperatorError(v_type, "~");
}

llvm::Value *Compiler::create_subscription(llvm::Value *list, llvm::Value *offset) {
  llvm::Type *list_type = this->list_infos[list].type;
  llvm::Value *element_ptr = this->builder.CreateGEP(list_type, list, offset);

  return this->create_load(list_type, element_ptr);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_add(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "+");

  if (lhs_type == float_type)
    // If lhs is a float do straightforward floating point addition
    return this->builder.CreateFAdd(lhs, rhs);
  else if (rhs_type == float_type)
    // If lhs is not a float but rhs is we need to cast lhs to a float before we can add
    return this->builder.CreateFAdd(this->int_to_float(lhs), rhs);
  else
    // If neither lhs nor rhs are floats they are both ints (verified by guard clause above)
    return this->builder.CreateAdd(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_sub(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "-");

  if (lhs_type == float_type)
    return this->builder.CreateFSub(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFSub(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateSub(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_mul(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "*");

  if (lhs_type == float_type)
    return this->builder.CreateFMul(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFMul(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateMul(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_div(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "/");

  if (lhs_type == float_type)
    return this->builder.CreateFDiv(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFDiv(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateSDiv(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_lt(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "<");

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULT(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpULT(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpULT(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_le(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "<=");

  if (lhs_type->isDoubleTy())
    return this->builder.CreateFCmpULE(lhs, rhs);
  else if (rhs_type->isDoubleTy())
    return this->builder.CreateFCmpULE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpULE(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_eq(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();
  const llvm::Type *bool_type = this->get_bool_type();

  if (lhs_type == bool_type && rhs_type == bool_type)
    return this->builder.CreateICmpEQ(lhs, rhs);

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "==");

  if (lhs_type == float_type)
    return this->builder.CreateFCmpUEQ(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFCmpUEQ(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpEQ(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_neq(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();
  const llvm::Type *bool_type = this->get_bool_type();

  if (lhs_type == bool_type && rhs_type == bool_type)
    return this->builder.CreateICmpNE(lhs, rhs);

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, "~=");

  if (lhs_type == float_type)
    return this->builder.CreateFCmpUNE(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFCmpUNE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpNE(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_gt(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, ">");

  if (lhs_type == float_type)
    return this->builder.CreateFCmpUGT(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFCmpUGT(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpSGT(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_ge(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *int_type = this->get_int_type();
  const llvm::Type *float_type = this->get_float_type();

  if ((lhs_type != float_type && lhs_type != int_type) ||
      (rhs_type != float_type && rhs_type != int_type))
    return BinaryOperatorError(lhs_type, rhs_type, ">=");

  if (lhs_type == float_type)
    return this->builder.CreateFCmpUGE(lhs, rhs);
  else if (rhs_type == float_type)
    return this->builder.CreateFCmpUGE(this->int_to_float(lhs), rhs);
  else
    return this->builder.CreateICmpSGE(lhs, rhs);
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_and(llvm::Value *lhs,
                                                                      llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *bool_type = this->get_bool_type();

  if (lhs_type == bool_type && rhs_type == bool_type)
    return this->builder.CreateAnd(lhs, rhs);
  else
    return BinaryOperatorError(lhs_type, rhs_type, "and");
}

std::variant<llvm::Value *, BinaryOperatorError> Compiler::create_or(llvm::Value *lhs,
                                                                     llvm::Value *rhs) {
  const llvm::Type *lhs_type = lhs->getType();
  const llvm::Type *rhs_type = rhs->getType();
  const llvm::Type *bool_type = this->get_bool_type();

  if (lhs_type == bool_type && rhs_type == bool_type)
    return this->builder.CreateOr(lhs, rhs);
  else
    return BinaryOperatorError(lhs_type, rhs_type, "or");
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

bool Compiler::is_externally_defined(const llvm::Function *function) const {
  // TODO: more sophisticated structure, probably a hashmap from name to functions
  const std::string function_name = function->getName().str();
  if (function_name == "printf" || function_name == "malloc")
    return true;
  else
    return false;
}

std::variant<llvm::CallInst *, ArgumentCountError>
Compiler::create_extern_call(llvm::Function *function,
                             const std::vector<llvm::Value *> &arguments) {
  if (auto error = ArgumentCountError::check(function, arguments.size()); error.has_value())
    return error.value();

  return this->builder.CreateCall(function, arguments);
}

std::variant<llvm::CallInst *, ArgumentCountError>
Compiler::create_userdefined_call(llvm::Function *function, std::vector<llvm::Value *> &arguments) {
  if (auto error = ArgumentCountError::check(function, arguments.size() + 1); error.has_value())
    return error.value();

  // Last argument is the closure struct
  const llvm::Argument *closure_arg = function->getArg(function->arg_size() - 1);
  llvm::Type *closure_type = closure_arg->getType();
  if (auto closure_type_casted = llvm::dyn_cast<llvm::StructType>(closure_type)) {
    arguments.push_back(this->create_closure_argument(closure_type_casted));

    return this->builder.CreateCall(function, arguments);
  } else {
    assert(false && "last argument to a user defined function should always be the closure");
  }
}

std::variant<llvm::CallInst *, ArgumentCountError>
Compiler::create_call(llvm::Function *function, std::vector<llvm::Value *> &arguments) {
  // TODO: probably should do type checking here
  if (this->is_externally_defined(function))
    return this->create_extern_call(function, arguments);
  else
    return this->create_userdefined_call(function, arguments);
}

std::variant<llvm::Value *, NameError> Compiler::get_value(const std::string &name) {
  if (auto error = NameError::check(*this->current_scope, name); error.has_value())
    return error.value();

  auto existing = this->current_scope->lookup(name);
  assert(existing.has_value() && "lookup failure should be caught by previous error checking");

  // Could add needs_loading to binding struct instead of `isa`. Reason to load LoadInst values is
  // because these are loaded pointers from closure argument, this is not really clear by the
  // current implementation since a LoadInst really could be anything
  // TODO: This is a little messy
  if (llvm::isa<llvm::AllocaInst>(existing->value) || llvm::isa<llvm::LoadInst>(existing->value)) {
    // This is for lists - need to copy over list info for loaded pointer
    if (llvm::isa<llvm::AllocaInst>(existing->value) && existing->value->getType()->isPointerTy()) {
      llvm::LoadInst *load = this->create_load(existing->type, existing->value);
      this->list_infos[load] = this->list_infos[existing->value];
      // HERE - NEED TO LOAD POINTER AND THEN PUT IN LIST INFO BEFORE RETURNING IT
      return load;
    } else {
      return this->create_load(existing->type, existing->value);
    }
  } else
    return existing->value;
}

} // namespace Kebab
