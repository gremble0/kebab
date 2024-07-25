#include "logging/Logger.hpp"

namespace Kebab {
namespace Logger {

void set_stream(std::ostream &new_stream) {
  std::lock_guard<std::mutex> lock(mutex);
  stream = &new_stream;
}

void silence() {
  std::lock_guard<std::mutex> lock(mutex);
  stream = &null_stream;
}

void log(const std::string &message) {
  std::string indent(indent_depth, ' ');
  *stream << indent << message << std::endl;
}

void log_with_indent(const std::string &message) {
  log(message);
  ++indent_depth;
}

void log_with_dedent(const std::string &message) {
  --indent_depth;
  log(message);
}

} // namespace Logger
} // namespace Kebab
