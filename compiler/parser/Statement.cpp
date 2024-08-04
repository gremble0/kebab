#include <cassert>
#include <optional>
#include <string>
#include <variant>

#include "compiler/Compiler.hpp"
#include "lexer/Token.hpp"
#include "parser/Constructor.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"

namespace Kebab::Parser {

std::unique_ptr<DefinitionStatement> DefinitionStatement::parse(Lexer &lexer) {
  auto definition = std::make_unique<DefinitionStatement>();
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
  // TODO: can use this more in constructor::compile methods, but this is still kinda shit
  this->constructor->name = this->name;
  llvm::Value *variable_value = this->constructor->compile(compiler);

  // meh
  if (auto *function = llvm::dyn_cast<llvm::Function>(variable_value)) {
    return function;
  } else {
    llvm::Type *declared_type = this->constructor->get_type()->get_llvm_type(compiler);
    const llvm::Type *actual_type = variable_value->getType();
    // Types are interned so pointer comparison is sufficient
    if (actual_type != declared_type)
      this->type_error({declared_type}, actual_type);

    std::variant<llvm::AllocaInst *, RedefinitionError> local = compiler.create_definition(
        this->name, static_cast<llvm::Constant *>(variable_value), declared_type, this->is_mutable);

    if (std::holds_alternative<llvm::AllocaInst *>(local))
      return std::get<llvm::AllocaInst *>(local);
    else
      this->compiler_error(std::get<RedefinitionError>(local));
  }
}

std::unique_ptr<AssignmentStatement> AssignmentStatement::parse(Lexer &lexer) {
  auto assignment = std::make_unique<AssignmentStatement>();
  assignment->start_parsing(lexer, "<assignment-statement>");

  lexer.skip({Token::Type::SET});
  assignment->name = lexer.skip_name();
  lexer.skip({Token::Type::EQUALS});
  assignment->constructor = Constructor::parse(lexer);

  assignment->finish_parsing(lexer, "</assignment-statement>");
  return assignment;
}

llvm::Value *AssignmentStatement::compile(Compiler &compiler) const {
  this->constructor->name = this->name;
  llvm::Value *variable_value = this->constructor->compile(compiler);

  if (auto *function = llvm::dyn_cast<llvm::Function>(variable_value)) {
    return function;
  } else {
    llvm::Type *declared_type = this->constructor->get_type()->get_llvm_type(compiler);
    const llvm::Type *actual_type = variable_value->getType();
    if (actual_type != declared_type)
      this->type_error({declared_type}, actual_type);

    auto result = compiler.create_assignment(
        this->name, static_cast<llvm::Constant *>(variable_value), declared_type);
    if (std::holds_alternative<llvm::AllocaInst *>(result))
      return std::get<llvm::AllocaInst *>(result);
    else if (std::holds_alternative<ImmutableAssignmentError>(result))
      this->compiler_error(std::get<ImmutableAssignmentError>(result));
    else
      this->compiler_error(std::get<AssignNonExistingError>(result));
  }
}

std::unique_ptr<ExpressionStatement> ExpressionStatement::parse(Lexer &lexer) {
  auto expression = std::make_unique<ExpressionStatement>();
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
    using enum Token::Type;
  case DEF:
    statement = DefinitionStatement::parse(lexer);
    break;

  case SET:
    statement = AssignmentStatement::parse(lexer);
    break;

  case INT_LITERAL:
  case FLOAT_LITERAL:
  case CHAR_LITERAL:
  case STRING_LITERAL:
  case NAME:
  // Cond expressions (if/elif/else)
  case IF:
  // Factor prefixes
  case PLUS:
  case MINUS:
  case MULT:
  case DIV:
  case NOT:
  // Booleans
  case TRUE:
  case FALSE:
  // Inner expression wrapped in parens, e.g. `(1 + 2)`
  case LPAREN:
  // Lists e.g. `[x, y, z]`
  case LBRACKET:
    statement = ExpressionStatement::parse(lexer);
    break;

  default:
    AstNode::parser_error(
        std::string("unexpected token '") + lexer.peek()->to_string_short() + '\'', lexer);
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

} // namespace Kebab::Parser
