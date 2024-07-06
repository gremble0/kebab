#include <gtest/gtest.h>
#include <iostream>
#include <string>

#include "../lexer/Lexer.hpp"
#include "Files.hpp"

namespace Kebab {
namespace Test {

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

static void replace_one(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string log_path = "lexer-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({ Lexer l(source_path); });

  {
    std::ofstream log_file(log_path);
    Lexer l(source_path);
    while (l.cur_token.type != Token::Type::END_OF_FILE) {
      log_file << l.cur_token.to_string() + '\n';
      l.advance();
    }
  }

  std::ifstream log_file(log_path);
}

void replace_expected() {
  replace_one("comments");
  replace_one("comparisons");
  replace_one("constructors");
  replace_one("operators");
  replace_one("empty");
  replace_one("escape-sequences");
  replace_one("const-and-mut");

  std::cout << "Replaced expected lexer output\n";
}

} // namespace Test
} // namespace Kebab
