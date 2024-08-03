#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include "llvm/IR/Function.h"
#include <cstddef>

struct ArgumentCountError {
  size_t expected;
  size_t actual;

  // TODO: to_string with private fields?
  static std::optional<ArgumentCountError> check_argument_count(llvm::Function *function,
                                                                size_t argument_count);
};

#endif
