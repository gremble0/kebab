#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

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

namespace Kebab {
// Can't unclude RootNode due to circular imports so have to declare independently
namespace Parser {
class RootNode;
class Constructor;
} // namespace Parser

class Compiler {
private:
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;
  std::shared_ptr<Scope> current_scope;

  void save_module(const std::string &path) const;

  void load_printf();
  void load_globals();

public:
  Compiler()
      : context(), module("kebab", context), builder(context),
        current_scope(std::make_shared<Scope>()) {}

  void compile(std::unique_ptr<Parser::RootNode> root);
  [[noreturn]] void error(const std::string &message) const;

  llvm::IntegerType *get_int_type() { return this->builder.getInt64Ty(); }

  llvm::Type *get_float_type() { return this->builder.getDoubleTy(); }

  llvm::IntegerType *get_char_type() { return this->builder.getInt8Ty(); }

  llvm::PointerType *get_string_type() { return this->builder.getInt8Ty()->getPointerTo(); }

  llvm::IntegerType *get_bool_type() { return this->builder.getInt1Ty(); }

  llvm::Type *get_void_type() { return this->builder.getVoidTy(); }

  std::optional<llvm::Value *> get_value(const std::string &name);
  llvm::Function *get_current_function() const;

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

  llvm::Function *define_function(llvm::FunctionType *type, const std::string &name,
                                  const std::unique_ptr<Parser::Constructor> &body);

  llvm::Function *declare_function(llvm::FunctionType *type, const std::string &name);

  llvm::AllocaInst *create_local(const std::string &name, llvm::Constant *init, llvm::Type *type);

  llvm::Value *create_neg(llvm::Value *v) {
    // TODO: type check - if not int or bool error
    return this->builder.CreateNeg(v);
  }

  llvm::Value *create_add(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_sub(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_mul(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_div(llvm::Value *lhs, llvm::Value *rhs);

  llvm::Value *create_lt(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_le(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_eq(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_neq(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_gt(llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value *create_ge(llvm::Value *lhs, llvm::Value *rhs);

  llvm::Value *create_and(llvm::Value *lhs, llvm::Value *rhs) {
    return this->builder.CreateAnd(lhs, rhs);
  }

  llvm::Value *create_or(llvm::Value *lhs, llvm::Value *rhs) {
    return this->builder.CreateOr(lhs, rhs);
  }

  llvm::Value *create_not(llvm::Value *v) { return this->builder.CreateNot(v); }

  llvm::BasicBlock *create_basic_block(llvm::Function *parent, const std::string &name = "");
  llvm::BranchInst *create_branch(llvm::BasicBlock *destination);
  llvm::BranchInst *create_cond_branch(llvm::Value *condition, llvm::BasicBlock *true_destination,
                                       llvm::BasicBlock *false_destination);
  llvm::PHINode *create_phi(llvm::Type *type,
                            std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> incoming);

  llvm::CallInst *create_call(llvm::Function *function, std::vector<llvm::Value *> arguments) {
    return this->builder.CreateCall(function, arguments);
  }

  void set_insert_point(llvm::BasicBlock *block) { this->builder.SetInsertPoint(block); }
};

} // namespace Kebab

#endif
