#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Disable unused parameter warnings for llvm headers
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#pragma clang diagnostic pop

#include "Scope.hpp"

namespace Kebab::Parser {
class RootNode;
class Constructor;
class FunctionParameter;
} // namespace Kebab::Parser

namespace Kebab {
// Can't unclude RootNode due to circular imports so have to declare independently

class Compiler {
private:
  llvm::LLVMContext context;
  llvm::Module mod; // `module` is a reserved word as of c++20 so cannot use that
  llvm::IRBuilder<> builder;
  std::shared_ptr<Scope> current_scope = std::make_shared<Scope>();

  void save_module(const std::string &path) const;

  void load_printf();
  void load_globals();

  llvm::Value *int_to_float(llvm::Value *i);

  llvm::AllocaInst *create_alloca(const std::string &name, llvm::Constant *init, llvm::Type *type);
  llvm::AllocaInst *create_alloca(const std::string &name, const std::vector<llvm::Value *> &init,
                                  llvm::Type *type);

  llvm::Align get_alignment(llvm::Type *type) const;

public:
  Compiler() : mod("kebab", context), builder(context) {}

  void compile(std::unique_ptr<Parser::RootNode> root, const std::string &output_path);

  /// Type getters
  llvm::IntegerType *get_int_type() { return this->builder.getInt64Ty(); }

  llvm::Type *get_float_type() { return this->builder.getDoubleTy(); }

  llvm::IntegerType *get_char_type() { return this->builder.getInt8Ty(); }

  llvm::PointerType *get_string_type() { return this->builder.getInt8Ty()->getPointerTo(); }

  llvm::IntegerType *get_bool_type() { return this->builder.getInt1Ty(); }

  llvm::Type *get_void_type() { return this->builder.getVoidTy(); }

  /// Constructors for primitive literals
  llvm::ConstantInt *create_int(int64_t i) {
    return llvm::ConstantInt::get(this->builder.getInt64Ty(), i);
  }

  llvm::Constant *create_float(double_t f) {
    return llvm::ConstantFP::get(this->builder.getDoubleTy(), f);
  }

  llvm::ConstantInt *create_char(uint8_t c) {
    return llvm::ConstantInt::get(this->builder.getInt8Ty(), c);
  }

  llvm::Constant *create_string(const std::string &s) {
    return this->builder.CreateGlobalStringPtr(s);
  }

  llvm::Constant *create_bool(bool b) {
    return llvm::ConstantInt::get(this->builder.getInt1Ty(), b);
  }

  llvm::AllocaInst *create_list(const std::vector<llvm::Value *> &list, llvm::Type *type);

  /// Constructors for more complicated instructions
  llvm::Function *
  define_function(llvm::FunctionType *type, const std::string &name,
                  const Parser::Constructor &body,
                  const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters);
  llvm::Function *declare_function(llvm::FunctionType *type, const std::string &name);
  std::optional<llvm::AllocaInst *> create_immutable(const std::string &name, llvm::Constant *init,
                                                     llvm::Type *type);
  std::optional<llvm::AllocaInst *> create_mutable(const std::string &name, llvm::Constant *init,
                                                   llvm::Type *type);

  llvm::BasicBlock *create_basic_block(llvm::Function *parent, const std::string &name = "");

  llvm::BranchInst *create_branch(llvm::BasicBlock *destination);
  llvm::BranchInst *create_cond_branch(llvm::Value *condition, llvm::BasicBlock *true_destination,
                                       llvm::BasicBlock *false_destination);
  llvm::PHINode *
  create_phi(llvm::Type *type,
             const std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> &incoming);

  llvm::CallInst *create_call(llvm::Function *function,
                              const std::vector<llvm::Value *> &arguments) {
    return this->builder.CreateCall(function, arguments);
  }

  /// Unary mathematical operators
  std::optional<llvm::Value *> create_neg(llvm::Value *v); // -x

  /// Binary array operators
  std::optional<llvm::Value *> create_subscription(llvm::AllocaInst *array,
                                                   llvm::Value *offset); // x[y]

  /// Binary mathematical operators
  std::optional<llvm::Value *> create_add(llvm::Value *lhs, llvm::Value *rhs); // x + y
  std::optional<llvm::Value *> create_sub(llvm::Value *lhs, llvm::Value *rhs); // x - y
  std::optional<llvm::Value *> create_mul(llvm::Value *lhs, llvm::Value *rhs); // x * y
  std::optional<llvm::Value *> create_div(llvm::Value *lhs, llvm::Value *rhs); // x / y

  /// Binary comparison operators
  std::optional<llvm::Value *> create_lt(llvm::Value *lhs, llvm::Value *rhs);  // x < y
  std::optional<llvm::Value *> create_le(llvm::Value *lhs, llvm::Value *rhs);  // x <= y
  std::optional<llvm::Value *> create_eq(llvm::Value *lhs, llvm::Value *rhs);  // x == y
  std::optional<llvm::Value *> create_neq(llvm::Value *lhs, llvm::Value *rhs); // x ~= y
  std::optional<llvm::Value *> create_gt(llvm::Value *lhs, llvm::Value *rhs);  // x > y
  std::optional<llvm::Value *> create_ge(llvm::Value *lhs, llvm::Value *rhs);  // x >= y

  /// Binary logical operators
  std::optional<llvm::Value *> create_and(llvm::Value *lhs, llvm::Value *rhs); // x and y
  std::optional<llvm::Value *> create_or(llvm::Value *lhs, llvm::Value *rhs);  // x or y
  std::optional<llvm::Value *> create_not(llvm::Value *v);                     // ~x

  /// Setter wrappers
  void set_insert_point(llvm::BasicBlock *block) { this->builder.SetInsertPoint(block); }

  /// Scope wrappers and lookups
  std::optional<llvm::Value *> get_value(const std::string &name) const {
    return this->current_scope->lookup(name)->value;
  }

  void start_scope() { this->current_scope = std::make_shared<Scope>(this->current_scope); }

  void end_scope() { this->current_scope = this->current_scope->parent_or(this->current_scope); }

  llvm::Function *get_current_function() const {
    return this->builder.GetInsertBlock()->getParent();
  }
};

} // namespace Kebab

#endif
