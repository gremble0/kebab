#include <fstream>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "test/Files.hpp"
#include "gtest/gtest.h"

namespace Kebab {
namespace Test {

static void lex_file(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string log_path = "lexer-logs/" + basename + ".log";
  std::string expected_path = "lexer-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  {
    std::ofstream log_file(log_path);
    Logger::set_stream(log_file);

    Lexer lexer(source_path);
    while (lexer.cur_token->type != Token::Type::END_OF_FILE)
      lexer.advance();
  }

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(LexerTest, InitializesCorrectly) {
  Logger::silence();
  ASSERT_NO_FATAL_FAILURE({ Lexer lexer("lexer-source/comments.keb"); });
  ASSERT_DEATH({ Lexer lexer("non-existent-file"); }, "could not open file");

  Lexer lexer("lexer-source/comments.keb");
  ASSERT_NE(lexer.cur_token->type, Token::Type::ILLEGAL);
}

TEST(LexerTest, LexesCommentsKeb) {
  const std::string basename = "comments";
  lex_file(basename);
}

TEST(LexerTest, LexesComparisonsKeb) {
  const std::string basename = "comparisons";
  lex_file(basename);
}

TEST(LexerTest, LexesConstructorsKeb) {
  const std::string basename = "constructors";
  lex_file(basename);
}

TEST(LexerTest, LexesOperatorsKeb) {
  const std::string basename = "operators";
  lex_file(basename);
}

TEST(LexerTest, LexesConstAndMut) {
  const std::string basename = "const-and-mut";
  lex_file(basename);
}

TEST(LexerTest, LexesEscapeSequences) {
  const std::string basename = "escape-sequences";
  lex_file(basename);
}

TEST(LexerTest, LexesEmptyFile) {
  const std::string basename = "empty";
  lex_file(basename);
}

TEST(LexerTest, ErrorsWhenOutOfRange) {
  const std::string basename = "out-of-range";
  ASSERT_DEATH({ lex_file(basename); }, "number out of range");
}

TEST(LexerTest, ErrorsWhenUnterminatedCharLiteral) {
  const std::string basename = "unterminated-char-literal";
  ASSERT_DEATH({ lex_file(basename); }, "unterminated char literal");
}

TEST(LexerTest, ErrorsWhenMalformedCharLiteral) {
  const std::string basename = "malformed-char-literal";
  ASSERT_DEATH({ lex_file(basename); }, "malformed char literal");
}

} // namespace Test
} // namespace Kebab
