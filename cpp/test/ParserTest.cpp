#include <fstream>
#include <memory>
#include <string>

#include "Files.hpp"
#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/AstNode.hpp"
#include "parser/RootNode.hpp"
#include "gtest/gtest.h"

namespace Kebab {
namespace Test {

static void ASSERT_EXPECTED_PARSING(const std::string &basename) {
  std::string source_path = "parser-source/" + basename + ".keb";
  std::string log_path = "parser-logs/" + basename + ".log";
  std::string expected_path = "parser-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  {
    std::ofstream log_file(log_path);
    Logger::set_stream(log_file);

    Lexer lexer(source_path);
    std::unique_ptr<Parser::RootNode> root = Parser::RootNode::parse(lexer);
  }

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(ParserTest, ParsesEmptyFile) { ASSERT_EXPECTED_PARSING("empty"); }

TEST(ParserTest, ParsesFunctionConstructor) { ASSERT_EXPECTED_PARSING("function-constructor"); }

TEST(ParserTest, ParsesIf) { ASSERT_EXPECTED_PARSING("if"); }

TEST(ParserTest, ParsesListConstructor) { ASSERT_EXPECTED_PARSING("list-constructor"); }

TEST(ParserTest, ParsesOperators) { ASSERT_EXPECTED_PARSING("operators"); }

TEST(ParserTest, ParsesPrimitiveConstructors) { ASSERT_EXPECTED_PARSING("primitive-constructors"); }

TEST(ParserTest, ErrorsWhenMissingEquals) {
  ASSERT_DEATH({ ASSERT_EXPECTED_PARSING("missing-equals"); }, "unexpected token");
}

TEST(ParserTest, ErrorsWhenMissingConstructorType) {
  ASSERT_DEATH({ ASSERT_EXPECTED_PARSING("missing-constructor-type"); }, "unexpected token");
}

TEST(ParserTest, ErrorsWhenMissingConstructorTypeInFunction) {
  ASSERT_DEATH(
      { ASSERT_EXPECTED_PARSING("missing-constructor-type-in-function"); }, "unexpected token");
}

TEST(ParserTest, ErrorsWithGoodMessageWhenUnexpectedToken) {
  // ( and ) are escaped by \\ because otherwise they would be parsed as being regex symbols instead
  // of literal parens
  ASSERT_DEATH(
      { ASSERT_EXPECTED_PARSING("unexpected"); }, "unexpected token \\( expected , or \\)");
}

} // namespace Test
} // namespace Kebab
