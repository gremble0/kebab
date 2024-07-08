#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "llvm/IR/Value.h"

namespace Kebab {
namespace Parser {

class AstNode {
private:
  /**
   * @brief skip a token with an inner value and return it
   */
  template <typename T, Token::Type K> static T skip_value(Lexer &lexer) {
    expect(lexer, K);
    T value = std::get<T>(lexer.cur_token->value);
    lexer.advance();
    return value;
  }

protected:
  Span span;
  AstNode() = default;

  [[noreturn]] static void parser_error(const std::string &message, Lexer &lexer) {
    std::string pretty_position = lexer.pretty_position();
    std::string labeled_message = "parser-error: " + message;
    std::cerr << pretty_position << labeled_message << std::endl;

    exit(1);
  }

  [[noreturn]] void compiler_error(const std::string &message) const {
    std::string labeled_message = "compiler-error: " + message;
    std::cerr << labeled_message << std::endl;

    exit(1);
  }

  void start_parsing(Lexer &lexer, const std::string &node_name) {
    // maybe some #ifdef for logging (this would affect testing too)
    Logger::log_with_indent(node_name);
    this->span.start = lexer.position();
  }

  void finish_parsing(Lexer &lexer, const std::string &node_name) {
    // maybe some #ifdef for logging (this would affect testing too)
    Logger::log_with_dedent(node_name);
    this->span.end = lexer.position();
  }

  static void expect(Lexer &lexer, Token::Type type) {
    if (lexer.cur_token->type != type)
      parser_error("unexpected token '" + lexer.cur_token->to_string_short() + "' expected: '" +
                       Token::type_to_string(type) + '\'',
                   lexer);
  }

  // varargs?
  static void expect(Lexer &lexer, Token::Type either, Token::Type or_) {
    if (lexer.cur_token->type != either && lexer.cur_token->type != or_)
      parser_error("unexpected token '" + lexer.cur_token->to_string_short() + "' expected: '" +
                       Token::type_to_string(either) + "' or '" + Token::type_to_string(or_) + '\'',
                   lexer);
  }

  // Move to lexer?
  static void skip(Lexer &lexer, Token::Type type) {
    expect(lexer, type);
    lexer.advance();
  }

  static void skip(Lexer &lexer, Token::Type either, Token::Type or_) {
    expect(lexer, either, or_);
    lexer.advance();
  }

  static int64_t skip_int(Lexer &lexer) {
    return skip_value<int64_t, Token::Type::INT_LITERAL>(lexer);
  }

  static float_t skip_float(Lexer &lexer) {
    return skip_value<float_t, Token::Type::FLOAT_LITERAL>(lexer);
  }

  static uint8_t skip_char(Lexer &lexer) {
    return skip_value<uint8_t, Token::Type::CHAR_LITERAL>(lexer);
  }

  static std::string skip_string(Lexer &lexer) {
    return skip_value<std::string, Token::Type::STRING_LITERAL>(lexer);
  }

  static std::string skip_name(Lexer &lexer) {
    return skip_value<std::string, Token::Type::NAME>(lexer);
  }

  /**
   * @return whether the specified token was skipped
   */
  static bool ignore(Lexer &lexer, Token::Type type) {
    if (lexer.cur_token->type == type) {
      lexer.advance();
      return true;
    }
    return false;
  }

public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const = 0;
};

} // namespace Parser
} // namespace Kebab

#endif
