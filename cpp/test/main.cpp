#include "gtest/gtest.h"

int func() { return 1; }

TEST(test_func, returns1) { ASSERT_EQ(func(), 1); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
