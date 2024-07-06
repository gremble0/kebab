#ifndef KEBAB_LOGGER_HPP
#define KEBAB_LOGGER_HPP

#include <iostream>
#include <mutex>
#include <ostream>

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

  void internal_set_stream(std::ostream &stream) {
    std::lock_guard<std::mutex> lock(mutex);
    this->stream = &stream;
  }

  void internal_start_parsing(const std::string &node_name) {
    std::string indent = std::string(this->indent_depth, ' ');
    *this->stream << indent + '<' + node_name + ">\n";
    ++this->indent_depth;
  }

  void internal_end_parsing(const std::string &node_name) {
    --this->indent_depth;
    std::string indent = std::string(this->indent_depth, ' ');
    *this->stream << indent + '<' + node_name + ">\n";
  }

public:
  static void set_stream(std::ostream &stream) { get_logger().internal_set_stream(stream); }

  static void start_parsing(const std::string &node_name) {
    get_logger().internal_start_parsing(node_name);
  }

  static void end_parsing(const std::string &node_name) {
    get_logger().internal_end_parsing(node_name);
  }
};

#endif
