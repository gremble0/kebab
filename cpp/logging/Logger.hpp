#ifndef KEBAB_LOGGER_HPP
#define KEBAB_LOGGER_HPP

#include <iostream>
#include <mutex>
#include <ostream>

// Singleton class for logging
class Logger {
private:
  std::ostream *stream;
  std::mutex mutex; // Probably unnecessary but to future proof the class it is made thread safe
  size_t indent_depth;

  Logger() : stream(&std::cout), indent_depth(0) {}
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  static Logger &get_logger() {
    static Logger instance;
    return instance;
  }

  void set_stream_(std::ostream &stream) {
    std::lock_guard<std::mutex> lock(mutex);
    this->stream = &stream;
  }

  void log_(const std::string &message) const {
    std::string indent = std::string(this->indent_depth, ' ');
    *this->stream << indent + message << std::endl;
  }

  void log_with_indent_(const std::string &message) {
    log_(message);
    ++this->indent_depth;
  }

  void log_with_dedent_(const std::string &message) {
    --this->indent_depth;
    log_(message);
  }

public:
  static void set_stream(std::ostream &stream) { get_logger().set_stream_(stream); }

  static void log(const std::string &message) { get_logger().log_(message); }

  static void log_with_indent(const std::string &message) {
    get_logger().log_with_indent_(message);
  }

  static void log_with_dedent(const std::string &message) {
    get_logger().log_with_dedent_(message);
  }
};

#endif
