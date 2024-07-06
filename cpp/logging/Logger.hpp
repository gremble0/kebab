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

  Logger() : stream(&std::cout) {}
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

  void internal_log(const std::string &message) {
    std::lock_guard<std::mutex> lock(mutex);
    *stream << message << std::endl;
  }

public:
  static void set_stream(std::ostream &stream) { get_logger().internal_set_stream(stream); }

  static void start_parsing(const std::string &node_name) {
    static Logger instance;
    std::string indent = std::string(instance.indent_depth, ' ');
    *instance.stream << indent + '<' + node_name + ">\n";
    ++instance.indent_depth;
  }

  static void end_parsing(const std::string &node_name) {
    static Logger instance;
    --instance.indent_depth;
    std::string indent = std::string(instance.indent_depth, ' ');
    *instance.stream << indent + '<' + node_name + "/>\n";
  }
};

#endif
