#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <memory>
#include <string>

// Disable unused parameter warnings for llvm headers
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#pragma clang diagnostic pop

namespace Kebab {
namespace Parser {
class RootNode;
}
namespace Compiler {

class Compiler {
private:
  void save_module(const std::string &path);

  void load_printf();
  void load_globals();

  void start_main();
  void stop_main();

public:
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;

  Compiler()
      : context(llvm::LLVMContext()), module(llvm::Module("kebab", context)),
        builder(llvm::IRBuilder<>(context)) {}

  void compile(std::unique_ptr<Parser::RootNode> root);

  llvm::AllocaInst *create_variable(llvm::Type *type, const std::string &name,
                                    std::optional<llvm::Value *> init_value = std::nullopt);
};

} // namespace Compiler
} // namespace Kebab

#endif
