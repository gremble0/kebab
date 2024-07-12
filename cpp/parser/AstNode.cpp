#include <iostream>
#include <string>

#include "logging/Logger.hpp"
#include "parser/AstNode.hpp"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

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

std::string AstNode::where() const {
  std::string file_coordinates = this->path + ':' + std::to_string(this->span.start.line) + ':' +
                                 std::to_string(this->span.start.col) + '\n';
  std::string line = this->getnline(this->path, this->span.start.line) + '\n';
  std::string line_cursor =
      this->span.start.col > 0 ? std::string(this->span.start.col - 1, ' ') + "^\n" : "^\n";

  return file_coordinates + line + line_cursor;
}

// TODO: remove this function
[[noreturn]] void AstNode::parser_error(const std::string &message, Lexer &lexer) {
  std::string pretty_position = lexer.pretty_position();
  std::string labeled_message = "parser-error: " + message;

  std::cerr << pretty_position << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::unrecognized_type_error(const std::string &type_name) const {
  std::string where = this->where();
  std::string labeled_message = "unrecognized-type-error: '" + type_name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::type_error(const llvm::Type *expected, const llvm::Type *actual) const {
  std::string message;
  llvm::raw_string_ostream stream(message);
  stream << this->where();
  stream << "type-error: actual type '";
  actual->print(stream);
  stream << "' does not match expected type '";
  expected->print(stream);
  stream << '\'';

  std::cerr << message << std::endl;

  exit(1);
}

void AstNode::start_parsing(Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_indent(node_name);
  this->span.start = lexer.position();
  this->path = lexer.get_path();
}

void AstNode::finish_parsing(Lexer &lexer, const std::string &node_name) {
  // maybe some #ifdef for logging (this would affect testing too)
  Logger::log_with_dedent(node_name);
  this->span.end = lexer.position();
}

} // namespace Parser
} // namespace Kebab
