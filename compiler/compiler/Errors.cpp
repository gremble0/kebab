#include <format>

#include "Errors.hpp"

std::optional<ArgumentCountError> ArgumentCountError::check_argument_count(llvm::Function *function,
                                                                           size_t argument_count) {
  size_t function_arg_size = function->arg_size();

  if (function->isVarArg()) {
    if (argument_count < function_arg_size)
      // currently all variadic functions are extern meaning they dont take a closure so no -1
      return ArgumentCountError{function_arg_size, argument_count};
  } else {
    if (function_arg_size != argument_count)
      // -1 for closure (user shouldn't see that this is an argument to their function)
      return ArgumentCountError{function_arg_size - 1, argument_count - 1};
  }

  return std::nullopt;
}

std::string ArgumentCountError::to_string() const {
  return std::format("argument-count-error: called function expects {} arguments, but was given {}",
                     std::to_string(this->expected), std::to_string(this->actual));
}
