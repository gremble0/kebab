#include <fstream>

#include "../lexer/Lexer.hpp"
#include "Files.hpp"
#include "gtest/gtest.h"

TEST(LexerTest, InitializesCorrectly) {
  ASSERT_NO_FATAL_FAILURE({ Lexer l("lexer-source/comments.keb"); });
  ASSERT_DEATH({ Lexer l("non-existent-file"); }, "could not open file");

  Lexer l("lexer-source/comments.keb");
  ASSERT_EQ(l.cur_token.kind, Token::Kind::ILLEGAL);
  l.advance();
  ASSERT_FALSE(l.cur_token.kind == Token::Kind::ILLEGAL);
}

TEST(LexerTest, LexesCommentsKeb) {
  ASSERT_NO_FATAL_FAILURE({ Lexer l("lexer-source/comments.keb"); });

  {
    std::ofstream log_file("lexer-logs/comments.keb");
    Lexer l("lexer-source/comments.keb");
    while (l.cur_token.kind != Token::Kind::END_OF_FILE) {
      log_file << l.cur_token.to_string() + '\n';
      l.advance();
    }
  }

  std::ifstream expected_file("lexer-expected/comments.keb");
  std::ifstream log_file("lexer-logs/comments.keb");
  ASSERT_FILES_EQ(expected_file, log_file);
}
