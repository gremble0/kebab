#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
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

#include "compiler/Errors.hpp"
#include "compiler/Scope.hpp"

namespace Kebab::Parser {
// Forward declarations to avoid circular includes
class RootNode;
class Constructor;
class FunctionParameter;
} // namespace Kebab::Parser

namespace Kebab {

class Compiler {
private:
  llvm::LLVMContext context;
  llvm::Module mod; // `module` is a reserved word as of c++20 so cannot use that
  llvm::IRBuilder<> builder;

  std::shared_ptr<Scope> current_scope = std::make_shared<Scope>();
  std::unordered_map<std::string, llvm::Type *> primitive_types;

  struct ListInfo {
    llvm::Type *type;
    llvm::Value *size;
  };
  // Lists are heap allocated with opaque pointers so we need to store information about these
  // pointers before we lose it
  std::unordered_map<llvm::Value *, ListInfo> list_infos;

  void save_module(const std::string &path) const;

  void declare_malloc();
  void declare_printf();
  void declare_extern_functions();

  llvm::Value *int_to_float(llvm::Value *i);

  /// IRBuilder wrappers that fix alignment
  llvm::AllocaInst *create_alloca(const std::string &name, llvm::Value *init, llvm::Type *type);
  llvm::AllocaInst *create_alloca(const std::string &name, const std::vector<llvm::Value *> &init,
                                  llvm::Type *type);
  llvm::LoadInst *create_load(llvm::Type *type, llvm::Value *value);
  llvm::StoreInst *create_store(llvm::Value *init, llvm::Value *dest);

  llvm::Align get_alignment(llvm::Type *type) const;

  llvm::FunctionType *add_parameter(const llvm::FunctionType *function_type, llvm::Type *parameter);

  llvm::Value *create_closure_argument(llvm::StructType *closure_type);
  void load_arguments(const llvm::Function *function,
                      const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters);

  bool is_externally_defined(const llvm::Function *function) const;
  // Call an externally defined function that follows the C ABI
  std::variant<llvm::CallInst *, ArgumentCountError>
  create_extern_call(llvm::Function *function, const std::vector<llvm::Value *> &arguments);
  // Call a userdefined function that follows kebab function declaration style
  std::variant<llvm::CallInst *, ArgumentCountError>
  create_userdefined_call(llvm::Function *function, std::vector<llvm::Value *> &arguments);

public:
  Compiler() : mod("kebab", context), builder(context) {
    this->primitive_types["int"] = this->builder.getInt64Ty();
    this->primitive_types["float"] = this->builder.getDoubleTy();
    this->primitive_types["char"] = this->builder.getInt8Ty();
    this->primitive_types["string"] = this->builder.getInt8Ty()->getPointerTo();
    this->primitive_types["bool"] = this->builder.getInt1Ty();
    this->primitive_types["void"] = this->builder.getVoidTy();
  }

  void compile(std::unique_ptr<Parser::RootNode> root, const std::string &output_path);

  /// Type getters
  std::variant<llvm::Type *, UnrecognizedTypeError>
  get_primitive_type(const std::string &type_name) const;

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

  llvm::Value *create_list(const std::vector<llvm::Value *> &list, llvm::Type *type);

  /// Constructors for more complicated instructions
  llvm::StructType *create_closure_type();
  llvm::Function *
  define_function(const llvm::FunctionType *function_type, const std::string &name,
                  const Parser::Constructor &body,
                  const std::vector<std::unique_ptr<Parser::FunctionParameter>> &parameters);
  llvm::Function *declare_function(llvm::FunctionType *type, const std::string &name);

  std::variant<llvm::AllocaInst *, RedefinitionError>
  create_definition(const std::string &name, llvm::Value *init, bool is_mutable);
  std::variant<llvm::Value *, ImmutableAssignmentError, AssignNonExistingError, TypeError>
  create_assignment(const std::string &name, llvm::Value *init);

  llvm::BasicBlock *create_basic_block(llvm::Function *parent, const std::string &name = "");

  llvm::BranchInst *create_branch(llvm::BasicBlock *destination);
  llvm::BranchInst *create_cond_branch(llvm::Value *condition, llvm::BasicBlock *true_destination,
                                       llvm::BasicBlock *false_destination);
  llvm::PHINode *
  create_phi(llvm::Type *type,
             const std::vector<std::pair<llvm::Value *, llvm::BasicBlock *>> &incoming);

  // arguments is not const because the function may append the closure environment to the arguments
  // before calling the function
  std::variant<llvm::CallInst *, ArgumentCountError>
  create_call(llvm::Function *function, std::vector<llvm::Value *> &arguments);

  /// Unary mathematical operators
  std::variant<llvm::Value *, UnaryOperatorError> create_neg(llvm::Value *v);

  /// Unary logical operators
  std::variant<llvm::Value *, UnaryOperatorError> create_not(llvm::Value *v);

  /// Binary array operators
  // TODO: could also return operatorerror if offset is wrong type for example
  llvm::Value *create_subscription(llvm::Value *list, llvm::Value *offset);

  /// Binary mathematical operators
  std::variant<llvm::Value *, BinaryOperatorError> create_add(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_sub(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_mul(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_div(llvm::Value *lhs, llvm::Value *rhs);

  /// Binary comparison operators
  std::variant<llvm::Value *, BinaryOperatorError> create_lt(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_le(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_eq(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_neq(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_gt(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_ge(llvm::Value *lhs, llvm::Value *rhs);

  /// Binary logical operators
  std::variant<llvm::Value *, BinaryOperatorError> create_and(llvm::Value *lhs, llvm::Value *rhs);
  std::variant<llvm::Value *, BinaryOperatorError> create_or(llvm::Value *lhs, llvm::Value *rhs);

  /// Setter wrappers
  void set_insert_point(llvm::BasicBlock *block) { this->builder.SetInsertPoint(block); }

  /// Scope wrappers and lookups
  std::variant<llvm::Value *, NameError> get_value(const std::string &name);

  void start_scope() { this->current_scope = std::make_shared<Scope>(this->current_scope); }

  void end_scope() { this->current_scope = this->current_scope->parent_or(this->current_scope); }

  llvm::Function *get_current_function() const {
    return this->builder.GetInsertBlock()->getParent();
  }
};

} // namespace Kebab

#endif
