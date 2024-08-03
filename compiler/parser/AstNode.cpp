#include <format>
#include <initializer_list>
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

[[noreturn]] void AstNode::unreachable_error() const {
  std::cerr << "unreachable-error: reached unreachable branch during compilation (if you're seeing "
               "this there is a bug in the language implementation)"
            << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::nonhomogenous_list_error() const {
  std::string where = this->where();
  std::string labeled_message =
      "nonhomogenous-list-error: all elements in a list must be of the same type";

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::uncallable_error(const UncallableError &error) const {
  std::string where = this->where();
  std::string labeled_message = error.to_string();

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::argument_count_error(const ArgumentCountError &error) const {
  std::string where = this->where();
  std::string labeled_message = error.to_string();

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::immutable_assignment_error(const std::string &name) const {
  std::string where = this->where();
  std::string labeled_message =
      "immutable-assignment-error: cannot assign to immutable value '" + name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::unsubscriptable_error(const llvm::Type *subscriptee_type) const {
  std::string where = this->where();
  std::string subscriptee_type_string;
  llvm::raw_string_ostream subscriptee_type_stream(subscriptee_type_string);
  subscriptee_type->print(subscriptee_type_stream);
  std::string labeled_message = "unsubscriptable-error: variable with type '" +
                                subscriptee_type_string + "' cannot be subscripted";

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::index_error(const llvm::Value *capacity,
                                       const llvm::Value *index) const {
  std::string where = this->where();

  std::string capacity_string;
  llvm::raw_string_ostream capacity_stream(capacity_string);
  capacity->print(capacity_stream);

  std::string index_string;
  llvm::raw_string_ostream index_stream(index_string);
  index->print(index_stream);

  std::string labeled_message =
      std::format("index-error: index '{}' is out of range for list of size '{}'", index_string,
                  capacity_string);

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::reassignment_error(const std::string &name) const {
  std::string where = this->where();
  std::string labeled_message =
      "immutable-assignment-error: cannot reassign already declared value '" + name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::assign_nonexisting_error(const std::string &name) const {
  std::string where = this->where();
  std::string labeled_message =
      "immutable-assignment-error: cannot assign non declared value '" + name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::unrecognized_type_error(const std::string &type_name) const {
  std::string where = this->where();
  std::string labeled_message = "type-error: unrecognized type '" + type_name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::name_error(const std::string &name) const {
  std::string where = this->where();
  std::string labeled_message = "name-error: undeclared identifier '" + name + '\'';

  std::cerr << where << labeled_message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::type_error(std::initializer_list<const llvm::Type *> expected,
                                      const llvm::Type *actual) const {
  std::string message;
  llvm::raw_string_ostream stream(message);
  stream << this->where();
  stream << "type-error: unexpected type '";
  actual->print(stream);
  stream << "' expected ";

  int i = 0;
  int size = expected.size();
  for (const llvm::Type *type : expected) {
    stream << '\'';
    type->print(stream);
    stream << '\'';
    ++i;
    if (i == size)
      break;
    else if (i == size - 1)
      stream << " or ";
    else
      stream << ", ";
  }

  std::cerr << message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::operator_error(const std::vector<const llvm::Type *> &supported,
                                          const llvm::Type *actual,
                                          const std::string &operator_) const {
  std::string message;
  llvm::raw_string_ostream stream(message);
  stream << this->where();
  stream << "operator-error: unsupported type '";
  actual->print(stream);
  stream << "' for unary operator '" << operator_ << "' supported types are ";

  int i = 0;
  int size = supported.size();
  for (const llvm::Type *type : supported) {
    stream << '\'';
    type->print(stream);
    stream << '\'';
    ++i;
    if (i == size)
      break;
    else if (i == size - 1)
      stream << " and ";
    else
      stream << ", ";
  }

  std::cerr << message << std::endl;

  exit(1);
}

[[noreturn]] void AstNode::operator_error(const std::vector<const llvm::Type *> &supported,
                                          const llvm::Type *lhs, const llvm::Type *rhs,
                                          const std::string &operator_) const {
  std::string message;
  llvm::raw_string_ostream stream(message);
  stream << this->where();
  stream << "operator-error: unsupported types '";
  lhs->print(stream);
  stream << "' and '";
  rhs->print(stream);
  stream << "' for binary operator '" << operator_ << "' supported types are ";

  int i = 0;
  int size = supported.size();
  for (const llvm::Type *type : supported) {
    stream << '\'';
    type->print(stream);
    stream << '\'';
    ++i;
    if (i == size)
      break;
    else if (i == size - 1)
      stream << " and ";
    else
      stream << ", ";
  }

  std::cerr << message << std::endl;

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
