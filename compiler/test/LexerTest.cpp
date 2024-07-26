#include <fstream>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "test/Files.hpp"
#include "gtest/gtest.h"

namespace Kebab::Test {

static void lex_file(const std::string &log_path, const std::string &source_path) {
  std::ofstream log_file(log_path);
  Logger::set_stream(log_file);

  Lexer lexer(source_path);
  while (lexer.peek()->type != Token::Type::END_OF_FILE)
    lexer.advance();
}

static void ASSERT_EXPECTED_LEXING(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string log_path = "lexer-logs/" + basename + ".log";
  std::string expected_path = "lexer-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  lex_file(log_path, source_path);

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(LexerTest, InitializesCorrectly) {
  Logger::silence();
  ASSERT_NO_FATAL_FAILURE({ Lexer lexer("lexer-source/comments.keb"); });
  ASSERT_DEATH({ Lexer lexer("non-existent-file"); }, "could not open file");

  Lexer lexer("lexer-source/comments.keb");
  ASSERT_NE(lexer.peek()->type, Token::Type::END_OF_FILE);
}

TEST(LexerTest, LexesCommentsKeb) { ASSERT_EXPECTED_LEXING("comments"); }

TEST(LexerTest, LexesComparisonsKeb) { ASSERT_EXPECTED_LEXING("comparisons"); }

TEST(LexerTest, LexesConstructorsKeb) { ASSERT_EXPECTED_LEXING("constructors"); }

TEST(LexerTest, LexesOperatorsKeb) { ASSERT_EXPECTED_LEXING("operators"); }

TEST(LexerTest, LexesConstAndMut) { ASSERT_EXPECTED_LEXING("const-and-mut"); }

TEST(LexerTest, LexesEscapeSequences) { ASSERT_EXPECTED_LEXING("escape-sequences"); }

TEST(LexerTest, LexesEmptyFile) { ASSERT_EXPECTED_LEXING("empty"); }

TEST(LexerTest, ErrorsWhenOutOfRange) {
  ASSERT_DEATH({ ASSERT_EXPECTED_LEXING("out-of-range"); }, "number out of range");
}

TEST(LexerTest, ErrorsWhenUnterminatedCharLiteral) {
  ASSERT_DEATH(
      { ASSERT_EXPECTED_LEXING("unterminated-char-literal"); }, "unterminated char literal");
}

TEST(LexerTest, ErrorsWhenMalformedCharLiteral) {
  ASSERT_DEATH({ ASSERT_EXPECTED_LEXING("malformed-char-literal"); }, "malformed char literal");
}

TEST(LexerTest, ErrorsWhenIllegalToken) {
  ASSERT_DEATH({ ASSERT_EXPECTED_LEXING("illegal"); }, "illegal token");
}

} // namespace Kebab::Test
