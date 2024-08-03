#ifndef KEBAB_ERRORS_HPP
#define KEBAB_ERRORS_HPP

#include <cstddef>

struct ArgumentCountError {
  size_t expected;
  size_t actual;
};

#endif
