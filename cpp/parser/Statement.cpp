#include "Statement.hpp"
#include "lexer/Token.hpp"
#include "parser/Parser.hpp"

Statement *Statement::parse(Lexer &lexer) {
  // aaa
  switch (lexer.cur_token.kind) {
  case Token::Kind::DEF:
    return DefinitionStatement::parse(lexer);

  case Token::Kind::SET:
    return AssignmentStatement::parse(lexer);

  case Token::Kind::NAME:
  case Token::Kind::CHAR_LITERAL:
  case Token::Kind::STRING_LITERAL:
  case Token::Kind::INT_LITERAL:
    // Cond expressions (if/elif/else)
  case Token::Kind::IF:
    // Factor prefixes
  case Token::Kind::PLUS:
  case Token::Kind::MINUS:
  case Token::Kind::MULT:
  case Token::Kind::DIV:
  case Token::Kind::NOT:
    // Booleans
  case Token::Kind::TRUE:
  case Token::Kind::FALSE:
    // Inner expression wrapped in parens, e.g. `(1 + 2)`
  case Token::Kind::LPAREN:
    // Lists e.g. `[x, y, z]`
  case Token::Kind::LBRACKET:
    return ExpressionStatement::parse(lexer);

  default:
    AstNode::error("illegal syntax");
  }
}
