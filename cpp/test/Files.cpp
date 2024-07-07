#include <gtest/gtest.h>
#include <iostream>
#include <ostream>
#include <string>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "test/Files.hpp"

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

static void replace_one_lexer_expected(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string expected_path = "lexer-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  std::ofstream log_file(expected_path);
  Logger::set_stream(log_file);

  Lexer lexer(source_path);
  while (lexer.cur_token->type != Token::Type::END_OF_FILE)
    lexer.advance();
}

static void replace_lexer_expected() {
  replace_one_lexer_expected("comments");
  replace_one_lexer_expected("comparisons");
  replace_one_lexer_expected("constructors");
  replace_one_lexer_expected("operators");
  replace_one_lexer_expected("empty");
  replace_one_lexer_expected("escape-sequences");
  replace_one_lexer_expected("const-and-mut");

  std::cout << "Replaced expected lexer output" << std::endl;
}

void replace_expected() { replace_lexer_expected(); }

} // namespace Test
} // namespace Kebab
