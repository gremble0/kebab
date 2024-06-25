#ifndef SPAN_HPP
#define SPAN_HPP

#include <cstddef>
#include <string>

struct Position {
  size_t line, col;
  Position(size_t line, size_t col) : line(line), col(col) {}

  std::string to_string() const {
    return '[' + std::to_string(line) + ", " + std::to_string(col) + ']';
  }
};

struct Span {
  Position start, end;
  Span(Position start, Position end) : start(start), end(end) {}

  std::string to_string() const { return this->start.to_string() + " - " + this->end.to_string(); }
};

#endif
