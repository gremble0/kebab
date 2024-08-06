#include <cassert>
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
  this->load_globals();
  root->compile(*this);
  this->save_module(output_path);
}

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

std::variant<llvm::Type *, UnrecognizedTypeError>
Compiler::get_primitive_type(const std::string &type_name) const {
  if (auto maybe_error = UnrecognizedTypeError::check(this->primitive_types, type_name);
      maybe_error.has_value())
    return maybe_error.value();

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

void Compiler::load_parameters(
    const llvm::Function *function,
    const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters) {
  // Set parameter names and bring parameters into scope of function
  for (size_t i = 0, size = parameters.size(); i < size; ++i) {
    llvm::Argument *argument = function->getArg(i);
    argument->setName(parameters[i]->name);
    // TODO: mutability for parameters maybe with mut keyword for mutable params. This would have to
    // be changed in parser as well
    this->current_scope->put(parameters[i]->name, argument, argument->getType());
  }

  // Load and add fields of closure into scope
  // unsigned int because type is required by CreateExtractValue(), bindings.size - parameters.size
  // since bindings are expanded by parameters and we dont want to add these in the closure
  llvm::Argument *closure_arg = function->getArg(function->arg_size() - 1);
  closure_arg->setName("closure-env");
  std::vector<std::pair<const std::string &, Scope::Binding>> bindings =
      this->current_scope->bindings();
  for (unsigned int i = 0, size = bindings.size() - parameters.size(); i < size; ++i) {
    const auto &[name, binding] = bindings[i];
    llvm::Value *field = this->builder.CreateExtractValue(closure_arg, {i}, "closure-env:" + name);
    llvm::AllocaInst *field_pointer = this->create_alloca("closure-env:" + name + "-ptr", field,
                                                          field->getType()->getPointerTo());

    this->current_scope->put(name, field_pointer, binding.type, binding.is_mutable);
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
  this->load_parameters(function, parameters);
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
  // unsigned int because type is required by CreateExtractValue()
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

std::variant<llvm::AllocaInst *, RedefinitionError>
Compiler::create_definition(const std::string &name, llvm::Value *init, llvm::Type *type,
                            bool is_mutable) {
  if (auto maybe_error = RedefinitionError::check(*this->current_scope, name);
      maybe_error.has_value())
    return maybe_error.value();

  llvm::AllocaInst *local = this->create_alloca(name, init, type);
  this->current_scope->put(name, local, type, is_mutable);

  return local;
}

std::variant<llvm::AllocaInst *, ImmutableAssignmentError, AssignNonExistingError>
Compiler::create_assignment(const std::string &name, llvm::Value *init, llvm::Type *type) {
  if (auto maybe_error = AssignNonExistingError::check(*this->current_scope, name);
      maybe_error.has_value())
    return maybe_error.value();

  if (auto maybe_error = ImmutableAssignmentError::check(*this->current_scope, name);
      maybe_error.has_value())
    return maybe_error.value();

  auto existing = this->current_scope->lookup(name);
  assert(existing.has_value() && "lookup failure should be caught by previous error checking");
  assert(existing->is_mutable &&
         "assignment to immutable should be caught by previous error checking");

  auto local = llvm::dyn_cast<llvm::AllocaInst>(existing->value);
  assert(local != nullptr && "cannot assign to non stack allocated variable");

  // TODO: change the type of the assigned value in the scope with param `type`

  this->builder.CreateStore(init, local);

  return local;
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

std::variant<llvm::Value *, IndexError> Compiler::create_subscription(llvm::AllocaInst *list,
                                                                      llvm::Value *offset) {
  if (auto maybe_error = IndexError::check(list, offset); maybe_error.has_value())
    return maybe_error.value();

  llvm::Type *list_type = list->getAllocatedType();
  llvm::Value *element_ptr =
      this->builder.CreateInBoundsGEP(list_type, list, {this->create_int(0), offset});

  return this->builder.CreateLoad(list_type->getArrayElementType(), element_ptr);
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
  if (function->getName() == "printf")
    return true;
  else
    return false;
}

std::variant<llvm::CallInst *, ArgumentCountError>
Compiler::create_extern_call(llvm::Function *function,
                             const std::vector<llvm::Value *> &arguments) {
  if (auto maybe_error = ArgumentCountError::check(function, arguments.size());
      maybe_error.has_value())
    return maybe_error.value();

  return this->builder.CreateCall(function, arguments);
}

std::variant<llvm::CallInst *, ArgumentCountError>
Compiler::create_userdefined_call(llvm::Function *function, std::vector<llvm::Value *> &arguments) {
  if (auto maybe_error = ArgumentCountError::check(function, arguments.size() + 1);
      maybe_error.has_value())
    return maybe_error.value();

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
  if (auto maybe_error = NameError::check(*this->current_scope, name); maybe_error.has_value())
    return maybe_error.value();
  else {
    auto existing = this->current_scope->lookup(name);
    assert(existing.has_value() && "lookup failure should be caught by previous error checking");
    if (llvm::isa<llvm::AllocaInst>(existing->value))
      return this->builder.CreateLoad(existing->type, existing->value);
    else
      return existing->value;
  }
}

} // namespace Kebab
