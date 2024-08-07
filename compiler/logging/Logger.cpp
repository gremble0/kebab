#include <mutex>

#include "logging/Logger.hpp"

namespace Kebab::Logger {

static inline std::ostream null_stream(nullptr);
static inline std::ostream *stream = &std::cout;
// Probably unnecessary but to future proof the logger is made thread safe
static inline std::mutex mutex;
static inline size_t indent_depth = 0;

void set_stream(std::ostream &new_stream) {
  std::scoped_lock lock{mutex};
  stream = &new_stream;
}

void silence() {
  std::scoped_lock lock{mutex};
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

} // namespace Kebab::Logger
