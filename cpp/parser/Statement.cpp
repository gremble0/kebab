#include <cassert>
#include <optional>
#include <string>

#include "compiler/Compiler.hpp"
#include "lexer/Token.hpp"
#include "parser/Constructor.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"

namespace Kebab {
namespace Parser {

std::unique_ptr<DefinitionStatement> DefinitionStatement::parse(Lexer &lexer) {
  std::unique_ptr<DefinitionStatement> definition = std::make_unique<DefinitionStatement>();
  definition->start_parsing(lexer, "<definition-statement>");

  lexer.skip({Token::Type::DEF});

  if (lexer.peek()->type == Token::Type::MUT) {
    definition->is_mutable = true;
    lexer.advance();
  } else {
    definition->is_mutable = false;
  }

  definition->name = lexer.skip_name();
  lexer.skip({Token::Type::EQUALS});
  definition->constructor = Constructor::parse(lexer);

  definition->finish_parsing(lexer, "</definition-statement>");
  return definition;
}

llvm::Value *DefinitionStatement::compile(Compiler &compiler) const {
  llvm::Value *variable_value = this->constructor->compile(compiler);
  llvm::Type *declared_type = this->constructor->get_type()->get_llvm_type(compiler.builder);
  llvm::Type *actual_type = variable_value->getType();

  if (actual_type->getTypeID() != declared_type->getTypeID())
    this->type_error({declared_type}, actual_type);

  llvm::AllocaInst *local = compiler.create_local(
      this->name, static_cast<llvm::Constant *>(variable_value), declared_type);

  return local;
}

std::unique_ptr<AssignmentStatement> AssignmentStatement::parse(Lexer &lexer) {
  std::unique_ptr<AssignmentStatement> assignment = std::make_unique<AssignmentStatement>();
  assignment->start_parsing(lexer, "<assignment-statement>");

  lexer.skip({Token::Type::SET});
  assignment->name = lexer.skip_name();
  lexer.skip({Token::Type::EQUALS});
  assignment->constructor = Constructor::parse(lexer);

  assignment->finish_parsing(lexer, "</assignment-statement>");
  return assignment;
}

llvm::Value *AssignmentStatement::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function AssignmentStatement::compile");
}

std::unique_ptr<ExpressionStatement> ExpressionStatement::parse(Lexer &lexer) {
  std::unique_ptr<ExpressionStatement> expression = std::make_unique<ExpressionStatement>();
  expression->start_parsing(lexer, "<expression-statement>");

  expression->expression = Expression::parse(lexer);

  expression->finish_parsing(lexer, "</expression-statement>");
  return expression;
}

llvm::Value *ExpressionStatement::compile(Compiler &compiler) const {
  return this->expression->compile(compiler);
}

std::unique_ptr<Statement> Statement::parse(Lexer &lexer) {
  std::unique_ptr<Statement> statement;

  switch (lexer.peek()->type) {
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
    parser_error(std::string("unexpected token '") + lexer.peek()->to_string_short() + '\'', lexer);
  }

  return statement;
}

std::optional<std::unique_ptr<Statement>> Statement::try_parse_statement(Lexer &lexer) {
  switch (lexer.peek()->type) {
  case Token::Type::DEF:
  case Token::Type::SET:
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
    return Statement::parse(lexer);

  default:
    return std::nullopt;
  }
}

} // namespace Parser
} // namespace Kebab
