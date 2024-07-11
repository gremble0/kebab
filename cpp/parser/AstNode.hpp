#ifndef KEBAB_PARSER_HPP
#define KEBAB_PARSER_HPP

#include <cstdint>
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
   * @brief get the nth line from a file given by path
   */
  static std::string getnline(const std::string &path, size_t line_number);

  std::string where() const;

  /**
   * @brief skip a token with an inner value and return it
   */
  template <typename T, Token::Type K> static T skip_value(Lexer &lexer) {
    expect(lexer, K);
    T value = std::get<T>(lexer.get_token()->value);
    lexer.advance();
    return value;
  }

protected:
  Span span;
  std::string path;

  AstNode() = default;

  [[noreturn]] static void parser_error(const std::string &message, Lexer &lexer);
  [[noreturn]] void unrecognized_type_error(const std::string &type_name) const;
  [[noreturn]] void type_error(const llvm::Type *expected, const llvm::Type *actual) const;

  void start_parsing(Lexer &lexer, const std::string &node_name);
  void finish_parsing(Lexer &lexer, const std::string &node_name);

  // varargs?
  // move to lexer?
  static void expect(Lexer &lexer, Token::Type type);
  static void expect(Lexer &lexer, Token::Type either, Token::Type or_);
  static void skip(Lexer &lexer, Token::Type type);
  static void skip(Lexer &lexer, Token::Type either, Token::Type or_);
  static bool ignore(Lexer &lexer, Token::Type type);

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

public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
  virtual llvm::Value *compile(Compiler &compiler) const = 0;
};

} // namespace Parser
} // namespace Kebab

#endif
