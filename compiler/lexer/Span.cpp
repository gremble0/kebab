#include <format>

#include "Span.hpp"

namespace Kebab {

std::string Position::to_string() const {
  return std::format("[{}, {}]", std::to_string(line), std::to_string(col));
}

std::string Span::to_string() const {
  return std::format("{} - {}", this->start.to_string(), this->end.to_string());
}

} // namespace Kebab
