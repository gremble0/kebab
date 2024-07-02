#include <iostream>
#include <optional>

#include "compiler/Compiler.hpp"
#include "lexer/Token.hpp"
#include "parser/Constructor.hpp"
#include "parser/Parser.hpp"
#include "parser/Statement.hpp"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constants.h"

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

void DefinitionStatement::compile(Compiler::Compiler &compiler) const {
  std::cout << this->constructor->type << std::endl;
  // compiler.create_variable(this->constructor->type->get_llvm_type(compiler.builder), this->name);
}

std::unique_ptr<AssignmentStatement> AssignmentStatement::parse(Lexer &lexer) {
  start_parsing("assignment-statement");

  // TODO:

  end_parsing("assignment-statement");
  return nullptr;
}

void AssignmentStatement::compile(Compiler::Compiler &compiler) const {}

std::unique_ptr<ExpressionStatement> ExpressionStatement::parse(Lexer &lexer) {
  start_parsing("expression-statement");
  std::unique_ptr<ExpressionStatement> expression = std::make_unique<ExpressionStatement>();

  expression->expression = Expression::parse(lexer);

  end_parsing("expression-statement");
  return expression;
}

void ExpressionStatement::compile(Compiler::Compiler &compiler) const {}

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

std::optional<std::unique_ptr<Statement>> Statement::try_parse_expression_statement(Lexer &lexer) {
  switch (lexer.cur_token.type) {
  case Token::Type::INT_LITERAL:
  case Token::Type::FLOAT_LITERAL:
  case Token::Type::CHAR_LITERAL:
  case Token::Type::STRING_LITERAL:
  case Token::Type::NAME:
  case Token::Type::IF:
  case Token::Type::PLUS:
  case Token::Type::MINUS:
  case Token::Type::MULT:
  case Token::Type::DIV:
  case Token::Type::NOT:
  case Token::Type::TRUE:
  case Token::Type::FALSE:
  case Token::Type::LPAREN:
  case Token::Type::LBRACKET:
    return ExpressionStatement::parse(lexer);

  default:
    return std::nullopt;
  }
}

} // namespace Parser
} // namespace Kebab
