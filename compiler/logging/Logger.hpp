#ifndef KEBAB_LOGGER_HPP
#define KEBAB_LOGGER_HPP

#include <iostream>
#include <ostream>

namespace Kebab::Logger {

void set_stream(std::ostream &new_stream);
void silence();
void log(const std::string &message);
void log_with_indent(const std::string &message);
void log_with_dedent(const std::string &message);

} // namespace Kebab::Logger

#endif
