#include <string>

#include "Files.hpp"
#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/RootNode.hpp"
#include "gtest/gtest.h"

namespace Kebab {
namespace Test {

static void ASSERT_EXPECTED_COMPILATION(const std::string &basename) {
  std::string source_path = "compiler-source/" + basename + ".keb";
  std::string log_path = "compiler-logs/" + basename + ".ll";
  std::string expected_path = "compiler-expected/" + basename + ".ll";

  Logger::silence();

  ASSERT_NO_FATAL_FAILURE({ Lexer lexer(source_path); });

  Lexer lexer(source_path);
  std::unique_ptr<Parser::RootNode> root = Parser::RootNode::parse(lexer);
  Compiler compiler;
  compiler.compile(std::move(root), log_path);

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(CompilerTest, InitializesCorrectly) {
  Logger::silence();
  ASSERT_NO_FATAL_FAILURE({ Compiler compiler; });
}

TEST(CompilerTest, CompilesAndKeb) { ASSERT_EXPECTED_COMPILATION("and"); }

TEST(CompilerTest, CompilesBasicKeb) { ASSERT_EXPECTED_COMPILATION("basic"); }

TEST(CompilerTest, CompilesComparisonsKeb) { ASSERT_EXPECTED_COMPILATION("comparisons"); }

TEST(CompilerTest, CompilesFactorsKeb) { ASSERT_EXPECTED_COMPILATION("factors"); }

TEST(CompilerTest, CompilesFunctionsKeb) { ASSERT_EXPECTED_COMPILATION("functions"); }

TEST(CompilerTest, CompilesIfKeb) { ASSERT_EXPECTED_COMPILATION("if"); }

TEST(CompilerTest, CompilesMainKeb) { ASSERT_EXPECTED_COMPILATION("main"); }

TEST(CompilerTest, CompilesNotKeb) { ASSERT_EXPECTED_COMPILATION("not"); }

TEST(CompilerTest, CompilesOrAndKeb) { ASSERT_EXPECTED_COMPILATION("or-and"); }

TEST(CompilerTest, CompilesOrKeb) { ASSERT_EXPECTED_COMPILATION("or"); }

TEST(CompilerTest, CompilesTermsKeb) { ASSERT_EXPECTED_COMPILATION("terms"); }

} // namespace Test
} // namespace Kebab
