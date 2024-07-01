#ifndef KEBAB_COMPILER_HPP
#define KEBAB_COMPILER_HPP

#include <memory>

// #include "parser/RootNode.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

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
};

} // namespace Compiler
} // namespace Kebab

#endif
