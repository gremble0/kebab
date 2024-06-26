#include "Statement.hpp"
#include "Constructor.hpp"
#include "lexer/Token.hpp"
#include "parser/Parser.hpp"

DefinitionStatement *DefinitionStatement::parse(Lexer &lexer) {
  log_node_start("definition-statement");
  skip_token(lexer, Token::Kind::DEF);

  DefinitionStatement *def = new DefinitionStatement();

  if (lexer.cur_token.kind == Token::Kind::MUT) {
    def->is_mutable = true;
    lexer.advance();
  } else {
    def->is_mutable = false;
  }

  expect_token(lexer, Token::Kind::NAME);
  def->name = std::get<std::string>(lexer.cur_token.value);
  lexer.advance();

  skip_token(lexer, Token::Kind::EQUALS);

  def->constructor = Constructor::parse(lexer);

  log_node_end("definition-statement");
  return new DefinitionStatement();
}

AssignmentStatement *AssignmentStatement::parse(Lexer &lexer) {
  log_node_start("assignment-statement");

  // TODO:

  log_node_end("assignment-statement");
  return new AssignmentStatement();
}

ExpressionStatement *ExpressionStatement::parse(Lexer &lexer) {
  log_node_start("expression-statement");

  ExpressionStatement *expression = new ExpressionStatement();
  expression->expression = Expression::parse(lexer);

  log_node_end("expression-statement");
  return new ExpressionStatement();
}

Statement *Statement::parse(Lexer &lexer) {
  log_node_start("statement");
  Statement *statement;

  switch (lexer.cur_token.kind) {
  case Token::Kind::DEF:
    statement = DefinitionStatement::parse(lexer);
    break;

  case Token::Kind::SET:
    statement = AssignmentStatement::parse(lexer);
    break;

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
    statement = ExpressionStatement::parse(lexer);
    break;

  default:
    AstNode::error("illegal syntax");
  }

  lexer.advance();

  log_node_end("statement");
  return statement;
}
