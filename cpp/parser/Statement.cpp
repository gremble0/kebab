#include "Statement.hpp"
#include "Constructor.hpp"
#include "Parser.hpp"
#include "lexer/Token.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<DefinitionStatement> DefinitionStatement::parse(Lexer &lexer) {
  start_parsing("definition-statement");
  std::unique_ptr<DefinitionStatement> def = std::make_unique<DefinitionStatement>();

  skip(lexer, Token::Kind::DEF);

  if (lexer.cur_token.kind == Token::Kind::MUT) {
    def->is_mutable = true;
    lexer.advance();
  } else {
    def->is_mutable = false;
  }

  def->name = skip_name(lexer);
  skip(lexer, Token::Kind::EQUALS);
  def->constructor = Constructor::parse(lexer);

  end_parsing("definition-statement");
  return def;
}

std::unique_ptr<AssignmentStatement> AssignmentStatement::parse(Lexer &lexer) {
  start_parsing("assignment-statement");

  // TODO:

  end_parsing("assignment-statement");
  return nullptr;
}

std::unique_ptr<ExpressionStatement> ExpressionStatement::parse(Lexer &lexer) {
  start_parsing("expression-statement");
  std::unique_ptr<ExpressionStatement> expression = std::make_unique<ExpressionStatement>();

  expression->expression = Expression::parse(lexer);

  end_parsing("expression-statement");
  return expression;
}

std::unique_ptr<Statement> Statement::parse(Lexer &lexer) {
  start_parsing("statement");
  std::unique_ptr<Statement> statement;

  switch (lexer.cur_token.kind) {
  case Token::Kind::DEF:
    statement = DefinitionStatement::parse(lexer);
    break;

  case Token::Kind::SET:
    statement = AssignmentStatement::parse(lexer);
    break;

  case Token::Kind::INT_LITERAL:
  case Token::Kind::FLOAT_LITERAL:
  case Token::Kind::CHAR_LITERAL:
  case Token::Kind::STRING_LITERAL:
  case Token::Kind::NAME:
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

  end_parsing("statement");
  return statement;
}

} // namespace Parser
} // namespace Kebab
