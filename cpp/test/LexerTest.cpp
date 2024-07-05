#include <fstream>

#include "../lexer/Lexer.hpp"
#include "Files.hpp"
#include "gtest/gtest.h"

namespace Kebab {

static void lex_file_with_logs(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string log_path = "lexer-logs/" + basename + ".log";
  std::string expected_path = "lexer-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({ Lexer l(source_path); });

  {
    std::ofstream log_file(log_path);
    Lexer l(source_path);
    while (l.cur_token.type != Token::Type::END_OF_FILE) {
      log_file << l.cur_token.to_string() + '\n';
      l.advance();
    }
  }

  std::ifstream expected_file(expected_path);
  std::ifstream log_file(log_path);
  ASSERT_FILES_EQ(expected_file, log_file);
}

TEST(LexerTest, InitializesCorrectly) {
  ASSERT_NO_FATAL_FAILURE({ Lexer l("lexer-source/comments.keb"); });
  ASSERT_DEATH({ Lexer l("non-existent-file"); }, "could not open file");

  Lexer l("lexer-source/comments.keb");
  ASSERT_NE(l.cur_token.type, Token::Type::ILLEGAL);
}

TEST(LexerTest, LexesCommentsKeb) {
  const std::string basename = "comments";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesComparisonsKeb) {
  const std::string basename = "comparisons";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesConstructorsKeb) {
  const std::string basename = "constructors";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesOperatorsKeb) {
  const std::string basename = "operators";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesConstAndMut) {
  const std::string basename = "const-and-mut";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesEscapeSequences) {
  const std::string basename = "escape-sequences";
  lex_file_with_logs(basename);
}

TEST(LexerTest, LexesEmptyFile) {
  const std::string basename = "empty";
  lex_file_with_logs(basename);
}

TEST(LexerTest, ErrorsWhenOutOfRange) {
  const std::string basename = "out-of-range";
  ASSERT_DEATH({ lex_file_with_logs(basename); }, "number out of range");
}

TEST(LexerTest, ErrorsWhenUnterminatedCharLiteral) {
  const std::string basename = "unterminated-char-literal";
  ASSERT_DEATH({ lex_file_with_logs(basename); }, "unterminated char literal");
}

TEST(LexerTest, ErrorsWhenMalformedCharLiteral) {
  const std::string basename = "malformed-char-literal";
  ASSERT_DEATH({ lex_file_with_logs(basename); }, "malformed char literal");
}

} // namespace Kebab
