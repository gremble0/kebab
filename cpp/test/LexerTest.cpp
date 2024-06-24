#include <fstream>

#include "../lexer/Lexer.hpp"
#include "Files.hpp"
#include "gtest/gtest.h"

void test_file(const std::string &basename) {
  std::string source_path = "lexer-source/" + basename + ".keb";
  std::string log_path = "lexer-logs/" + basename + ".log";
  std::string expected_path = "lexer-expected/" + basename + ".log";

  std::cout << source_path << "\n";
  std::cout << log_path << "\n";
  std::cout << expected_path << "\n";

  ASSERT_NO_FATAL_FAILURE({ Lexer l(source_path); });

  {
    std::ofstream log_file(log_path);
    Lexer l(source_path);
    while (l.cur_token.kind != Token::Kind::END_OF_FILE) {
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
  ASSERT_EQ(l.cur_token.kind, Token::Kind::ILLEGAL);
  l.advance();
  ASSERT_NE(l.cur_token.kind, Token::Kind::ILLEGAL);
}

TEST(LexerTest, LexesCommentsKeb) {
  const std::string basename = "comments";
  test_file(basename);
}

TEST(LexerTest, LexesComparisonsKeb) {
  const std::string basename = "comparisons";
  test_file(basename);
}

TEST(LexerTest, LexesConstructorsKeb) {
  const std::string basename = "constructors";
  test_file(basename);
}

TEST(LexerTest, LexesOperatorsKeb) {
  const std::string basename = "operators";
  test_file(basename);
}
