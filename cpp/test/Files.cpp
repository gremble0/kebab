#include <gtest/gtest.h>
#include <string>

#include "Files.hpp"

void ASSERT_FILES_EQ(std::ifstream &f1, std::ifstream &f2) {
  while (!f1.eof() && !f2.eof()) {
    std::string f1_line;
    std::string f2_line;

    std::getline(f1, f1_line);
    std::getline(f2, f2_line);

    ASSERT_STREQ(f1_line.c_str(), f2_line.c_str());
  }

  ASSERT_TRUE(f1.eof() && f2.eof());
}
