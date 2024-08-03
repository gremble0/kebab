#include "Errors.hpp"

std::optional<ArgumentCountError> ArgumentCountError::check_argument_count(llvm::Function *function,
                                                                           size_t argument_count) {
  size_t function_arg_size = function->arg_size();

  if (function->isVarArg()) {
    if (argument_count < function_arg_size)
      return ArgumentCountError{function_arg_size, argument_count};
  } else {
    if (function_arg_size != argument_count)
      // -1 for closure (user shouldn't see that this is an argument to their function)
      return ArgumentCountError{function_arg_size - 1, argument_count - 1};
  }

  return std::nullopt;
}
