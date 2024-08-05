#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "compiler/Errors.hpp"
#include "logging/Logger.hpp"
#include "parser/AstNode.hpp"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

namespace Kebab::Parser {

std::string AstNode::getnline(const std::string &path, size_t line_number) {
  std::ifstream stream(path);
  std::string out;

  while (line_number > 0) {
    --line_number;
    std::getline(stream, out);
    if (stream.eof() || stream.fail())
      return "<reached unexpected end of file>";
  }

  return out;
}

std::string AstNode::where() const {
  std::string file_coordinates =
      std::format("{}:{}:{}\n", this->path, std::to_string(this->span.start.line),
                  std::to_string(this->span.start.col));
  std::string line = this->getnline(this->path, this->span.start.line) + '\n';
  std::string line_cursor =
      this->span.start.col > 0 ? std::string(this->span.start.col - 1, ' ') + "^\n" : "^\n";

  return file_coordinates + line + line_cursor;
}

// TODO: remove this function
[[noreturn]] void AstNode::parser_error(const std::string &message, const Lexer &lexer) {
  std::string pretty_position = lexer.pretty_position();
  std::string labeled_message = "parser-error: " + message;

  std::cerr << pretty_position << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::compiler_error(const CompilerError &error) const {
  std::string where = this->where();
  std::string labeled_message = error.to_string();

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::unreachable_error() const {
  std::cerr << "unreachable-error: reached unreachable branch during compilation (if you're seeing "
               "this there is a bug in the language implementation)"
            << std::endl;

  exit(1);
}

void AstNode::start_parsing(const Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_indent(node_name);
  this->span.start = lexer.position();
  this->path = lexer.get_path();
}

void AstNode::finish_parsing(const Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_dedent(node_name);
  this->span.end = lexer.position();
}

} // namespace Kebab::Parser
