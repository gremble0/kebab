#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "lexer/Lexer.hpp"

class AstNode {
public:
  virtual ~AstNode() = default;

  static std::unique_ptr<AstNode> parse(Lexer &lexer);
};

namespace Parser {

std::unique_ptr<AstNode> parse(Lexer &lexer);

}

#endif
