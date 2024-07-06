#include <cstring>
#include <iostream>

#include "test/Files.hpp"
#include "gtest/gtest.h"

using namespace Kebab;
using namespace Test;

static void handle_argv(char **argv) {
  if (strcmp(argv[1], "--replace-expected") == 0)
    replace_expected();
  else
    std::cerr << "Unrecognized flag: " + std::string(argv[1]) << std::endl;
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  if (argc > 1)
    handle_argv(argv);
  else
    return RUN_ALL_TESTS();
}
