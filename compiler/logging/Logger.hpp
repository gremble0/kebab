#ifndef KEBAB_LOGGER_HPP
#define KEBAB_LOGGER_HPP

#include <iostream>
#include <mutex>
#include <ostream>

namespace Kebab {
namespace Logger {

static inline std::ostream null_stream(0);
static inline std::ostream *stream = &std::cout;
// Probably unnecessary but to future proof the logger is made thread safe
static inline std::mutex mutex;
static inline size_t indent_depth = 0;

void set_stream(std::ostream &new_stream);
void silence();
void log(const std::string &message);
void log_with_indent(const std::string &message);
void log_with_dedent(const std::string &message);

} // namespace Logger
} // namespace Kebab

#endif
