#include <iostream>
#include <string>

#include "logging/Logger.hpp"
#include "parser/AstNode.hpp"

namespace Kebab {
namespace Parser {

std::string AstNode::getnline(const std::string &path, size_t line_number) {
  std::ifstream stream(path);
  std::string out;

  while (line_number > 0) {
    --line_number;
    std::getline(stream, out);
    if (stream.eof() || stream.fail())
      return "<reached unexpected end of file>";
  }

  // unnecessary?
  stream.close();

  return out;
}

[[noreturn]] void AstNode::parser_error(const std::string &message, Lexer &lexer) {
  std::string pretty_position = lexer.pretty_position();
  std::string labeled_message = "parser-error: " + message;

  std::cerr << pretty_position << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::compiler_error(const std::string &message) const {
  std::string where = this->path + ':' + std::to_string(this->span.start.line) + ':' +
                      std::to_string(this->span.start.col) + '\n';
  std::string line = this->getnline(this->path, this->span.start.line) + '\n';
  std::string line_cursor =
      this->span.start.col > 0 ? std::string(this->span.start.col - 1, ' ') + "^\n" : "^\n";
  std::string labeled_message = "compiler-error: " + message;

  std::cerr << where << line << line_cursor << labeled_message << std::endl;

  exit(1);
}

void AstNode::start_parsing(Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_indent(node_name);
  this->span.start = lexer.position();
  this->path = lexer.path;
}

void AstNode::finish_parsing(Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_dedent(node_name);
  this->span.end = lexer.position();
}

void AstNode::expect(Lexer &lexer, Token::Type type) {
  if (lexer.cur_token->type != type)
    parser_error("unexpected token '" + lexer.cur_token->to_string_short() + "' expected: '" +
                     Token::type_to_string(type) + '\'',
                 lexer);
}

void AstNode::expect(Lexer &lexer, Token::Type either, Token::Type or_) {
  if (lexer.cur_token->type != either && lexer.cur_token->type != or_)
    parser_error("unexpected token '" + lexer.cur_token->to_string_short() + "' expected: '" +
                     Token::type_to_string(either) + "' or '" + Token::type_to_string(or_) + '\'',
                 lexer);
}

void AstNode::skip(Lexer &lexer, Token::Type type) {
  expect(lexer, type);
  lexer.advance();
}

void AstNode::skip(Lexer &lexer, Token::Type either, Token::Type or_) {
  expect(lexer, either, or_);
  lexer.advance();
}

/**
 * @return whether the specified token was skipped
 */
bool AstNode::ignore(Lexer &lexer, Token::Type type) {
  if (lexer.cur_token->type == type) {
    lexer.advance();
    return true;
  }
  return false;
}

} // namespace Parser
} // namespace Kebab
