// TODO: KEBAB_ before #ifndef everywhere
#ifndef KEBAB_ATOM_HPP
#define KEBAB_ATOM_HPP

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "parser/Expression.hpp"
#include "parser/Parser.hpp"

class Atom : public AstNode {
public:
  enum Type {
    CHAR,
    STRING,
    INT,
    BOOL,
    NAME,
    INNER_EXPR, // nested expression like (1 + 2)
    LIST,       // [x, y, z]
  };
  Type type;

  std::variant<uint8_t, std::string, int64_t, bool, Expression *, std::vector<Expression *>> value;
};

#endif
