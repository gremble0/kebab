#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "compiler/Compiler.hpp"
#include "lexer/Lexer.hpp"
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
  AstNode() = default;

  [[noreturn]] static void error(const std::string &message) {
    std::cerr << "parser-error: " + message << std::endl;

    exit(1);
  }

  static void expect(Lexer &lexer, Token::Type type) {
    if (lexer.cur_token->type != type)
      error("unexpected token: '" + lexer.cur_token->to_string() + "' expected: '" +
            Token::type_to_string(type) + '\'');
  }

  // varargs?
  static void expect(Lexer &lexer, Token::Type either, Token::Type or_) {
    if (lexer.cur_token->type != either && lexer.cur_token->type != or_)
      error("unexpected token: '" + lexer.cur_token->to_string() + "' expected: '" +
            Token::type_to_string(either) + "' or '" + Token::type_to_string(or_) + '\'');
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

std::unique_ptr<RootNode> parse(Lexer &lexer);

} // namespace Parser
} // namespace Kebab

#endif
