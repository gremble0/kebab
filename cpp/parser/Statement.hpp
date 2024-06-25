#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "Parser.hpp"
#include <memory>

class Statement : public AstNode {
public:
  static std::unique_ptr<Statement> parse(Lexer &lexer);
};

#endif
