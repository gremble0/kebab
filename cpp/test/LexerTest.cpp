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
    while (lexer.get_token()->type != Token::Type::END_OF_FILE)
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
  ASSERT_NE(lexer.get_token()->type, Token::Type::ILLEGAL);
}

TEST(LexerTest, LexesCommentsKeb) { lex_file("comments"); }

TEST(LexerTest, LexesComparisonsKeb) { lex_file("comparisons"); }

TEST(LexerTest, LexesConstructorsKeb) { lex_file("constructors"); }

TEST(LexerTest, LexesOperatorsKeb) { lex_file("operators"); }

TEST(LexerTest, LexesConstAndMut) { lex_file("const-and-mut"); }

TEST(LexerTest, LexesEscapeSequences) { lex_file("escape-sequences"); }

TEST(LexerTest, LexesEmptyFile) { lex_file("empty"); }

TEST(LexerTest, ErrorsWhenOutOfRange) {
  ASSERT_DEATH({ lex_file("out-of-range"); }, "number out of range");
}

TEST(LexerTest, ErrorsWhenUnterminatedCharLiteral) {
  ASSERT_DEATH({ lex_file("unterminated-char-literal"); }, "unterminated char literal");
}

TEST(LexerTest, ErrorsWhenMalformedCharLiteral) {
  ASSERT_DEATH({ lex_file("malformed-char-literal"); }, "malformed char literal");
}

} // namespace Test
} // namespace Kebab
