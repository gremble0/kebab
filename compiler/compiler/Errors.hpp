#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include <cstddef>

#include "llvm/IR/Function.h"

class ArgumentCountError {
private:
  size_t expected;
  size_t actual;

  ArgumentCountError(size_t expected, size_t actual) : expected(expected), actual(actual){};

public:
  static std::optional<ArgumentCountError> check_argument_count(llvm::Function *function,
                                                                size_t argument_count);

  std::string to_string() const;
};

#endif
