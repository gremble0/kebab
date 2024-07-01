#include "Statement.hpp"
#include "Constructor.hpp"
#include "Parser.hpp"
#include "compiler/Compiler.hpp"
#include "lexer/Token.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<DefinitionStatement> DefinitionStatement::parse(Lexer &lexer) {
  start_parsing("definition-statement");
  std::unique_ptr<DefinitionStatement> def = std::make_unique<DefinitionStatement>();

  skip(lexer, Token::Type::DEF);

  if (lexer.cur_token.type == Token::Type::MUT) {
    def->is_mutable = true;
    lexer.advance();
  } else {
    def->is_mutable = false;
  }

  def->name = skip_name(lexer);
  skip(lexer, Token::Type::EQUALS);
  def->constructor = Constructor::parse(lexer);

  end_parsing("definition-statement");
  return def;
}

void DefinitionStatement::compile(Compiler &compiler) {}

std::unique_ptr<AssignmentStatement> AssignmentStatement::parse(Lexer &lexer) {
  start_parsing("assignment-statement");

  // TODO:

  end_parsing("assignment-statement");
  return nullptr;
}

void AssignmentStatement::compile(Compiler &compiler) {}

std::unique_ptr<ExpressionStatement> ExpressionStatement::parse(Lexer &lexer) {
  start_parsing("expression-statement");
  std::unique_ptr<ExpressionStatement> expression = std::make_unique<ExpressionStatement>();

  expression->expression = Expression::parse(lexer);

  end_parsing("expression-statement");
  return expression;
}

void ExpressionStatement::compile(Compiler &compiler) {}

std::unique_ptr<Statement> Statement::parse(Lexer &lexer) {
  start_parsing("statement");
  std::unique_ptr<Statement> statement;

  switch (lexer.cur_token.type) {
  case Token::Type::DEF:
    statement = DefinitionStatement::parse(lexer);
    break;

  case Token::Type::SET:
    statement = AssignmentStatement::parse(lexer);
    break;

  case Token::Type::INT_LITERAL:
  case Token::Type::FLOAT_LITERAL:
  case Token::Type::CHAR_LITERAL:
  case Token::Type::STRING_LITERAL:
  case Token::Type::NAME:
  // Cond expressions (if/elif/else)
  case Token::Type::IF:
  // Factor prefixes
  case Token::Type::PLUS:
  case Token::Type::MINUS:
  case Token::Type::MULT:
  case Token::Type::DIV:
  case Token::Type::NOT:
  // Booleans
  case Token::Type::TRUE:
  case Token::Type::FALSE:
  // Inner expression wrapped in parens, e.g. `(1 + 2)`
  case Token::Type::LPAREN:
  // Lists e.g. `[x, y, z]`
  case Token::Type::LBRACKET:
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
