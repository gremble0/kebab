#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/RootNode.hpp"
#include "test/Files.hpp"

namespace Kebab::Test {

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
  while (lexer.peek()->type != Token::Type::END_OF_FILE)
    lexer.advance();
}

static void replace_one_parser_expected(const std::string &basename) {
  std::string source_path = "parser-source/" + basename + ".keb";
  std::string expected_path = "parser-expected/" + basename + ".log";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  std::ofstream log_file(expected_path);
  Logger::set_stream(log_file);

  Lexer lexer(source_path);
  std::unique_ptr<Parser::RootNode> root = Parser::RootNode::parse(lexer);
}

static void replace_one_compiler_expected(const std::string &basename) {
  std::string source_path = "compiler-source/" + basename + ".keb";
  std::string expected_path = "compiler-expected/" + basename + ".ll";

  ASSERT_NO_FATAL_FAILURE({
    Logger::silence();
    Lexer lexer(source_path);
  });

  Lexer lexer(source_path);
  std::unique_ptr<Parser::RootNode> root = Parser::RootNode::parse(lexer);
  Compiler compiler;
  compiler.compile(std::move(root), expected_path);
}

// NOTE: These don't run on all the files because some of them are expected to fail and therefore do
// not have an expected file at all
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

static void replace_parser_expected() {
  replace_one_parser_expected("empty");
  replace_one_parser_expected("function-constructor");
  replace_one_parser_expected("if");
  replace_one_parser_expected("list-constructor");
  replace_one_parser_expected("operators");
  replace_one_parser_expected("primitive-constructors");
  replace_one_parser_expected("types");

  std::cout << "Replaced expected parser output" << std::endl;
}

static void replace_compiler_expected() {
  replace_one_compiler_expected("and");
  replace_one_compiler_expected("basic");
  replace_one_compiler_expected("comparisons");
  replace_one_compiler_expected("factors");
  replace_one_compiler_expected("functions");
  replace_one_compiler_expected("if");
  replace_one_compiler_expected("main");
  replace_one_compiler_expected("not");
  replace_one_compiler_expected("or-and");
  replace_one_compiler_expected("or");
  replace_one_compiler_expected("terms");
  replace_one_compiler_expected("inner-expression");
  replace_one_compiler_expected("closures");
  replace_one_compiler_expected("subscription");

  std::cout << "Replaced expected compiler output" << std::endl;
}

void replace_expected() {
  replace_lexer_expected();
  replace_parser_expected();
  replace_compiler_expected();
}

} // namespace Kebab::Test
