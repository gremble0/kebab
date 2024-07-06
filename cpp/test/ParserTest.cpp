#include <fstream>
#include <string>

#include "Files.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "gtest/gtest.h"

namespace Kebab {
namespace Test {

static void parse_file_with_logs(const std::string &basename) {
  std::string source_path = "parser-source/" + basename + ".keb";
  std::string log_path = "parser-logs/" + basename + ".log";
  std::string expected_path = "parser-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({ Lexer l(source_path); });

  {
    std::ofstream log_file(log_path);
    Lexer l(source_path);
    Parser::parse(l);
  }

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(ParserTest, ParsesIf) {}

} // namespace Test
} // namespace Kebab
