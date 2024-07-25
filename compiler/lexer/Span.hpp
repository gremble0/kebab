#ifndef KEBAB_SPAN_HPP
#define KEBAB_SPAN_HPP

#include <cstddef>
#include <string>

namespace Kebab {

struct Position {
  size_t line;
  size_t col;

  Position() : line(0), col(0) {}
  Position(size_t line, size_t col) : line(line), col(col) {}

  std::string to_string() const;
};

struct Span {
  Position start;
  Position end;

  Span() {}
  Span(Position start, Position end) : start(start), end(end) {}

  std::string to_string() const;
};

} // namespace Kebab

#endif
