#include <system_error>

#include "Compiler.hpp"
#include "llvm/Support/raw_ostream.h"

void Compiler::save_module(const std::string &path) {
  std::error_code error_code;
  llvm::raw_fd_stream fd(path, error_code);
  this->module->print(fd, nullptr);
}

void Compiler::compile(AstNode root) { this->save_module("./out.ll"); }
