#include <cassert>
#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Constructor.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"

namespace Kebab::Parser {

std::unique_ptr<Constructor> Constructor::parse(Lexer &lexer) {
  std::unique_ptr<Constructor> constructor;

  switch (lexer.peek()->type) {
  case Token::Type::FN:
    constructor = FunctionConstructor::parse(lexer);
    break;

  case Token::Type::LIST:
    constructor = ListConstructor::parse(lexer);
    break;

  case Token::Type::NAME:
    constructor = PrimitiveConstructor::parse(lexer);
    break;

  default:
    parser_error("unexpected token '" + lexer.peek()->to_string_short() + "' expected some type",
                 lexer);
  }

  return constructor;
}

void ListConstructor::parse_type(Lexer &lexer) {
  lexer.skip({Token::Type::LIST});
  lexer.skip({Token::Type::LPAREN});

  lexer.skip({Token::Type::LPAREN});
  this->type = std::make_unique<ListType>(); // TODO: make in constructor somewhere?
  this->type->content_type = Type::parse(lexer);
  lexer.skip({Token::Type::RPAREN});
}

void ListConstructor::parse_body(Lexer &lexer) {
  lexer.skip({Token::Type::FAT_RARROW});
  while (lexer.peek()->type != Token::Type::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  lexer.skip({Token::Type::RPAREN});
}

std::unique_ptr<ListConstructor> ListConstructor::parse(Lexer &lexer) {
  std::unique_ptr<ListConstructor> constructor = std::make_unique<ListConstructor>();
  constructor->start_parsing(lexer, "<list-constructor>");

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  constructor->finish_parsing(lexer, "</list-constructor>");
  return constructor;
}

llvm::Value *ListConstructor::compile(Compiler &compiler) const {
  compiler.start_scope();
  for (size_t i = 0; i < this->body.size() - 1; ++i)
    this->body[i]->compile(compiler);

  // Return value of each constructor is the last statement (which is an expression)
  llvm::Value *return_value = this->body.back()->compile(compiler);
  compiler.end_scope();

  return return_value;
}

std::unique_ptr<FunctionParameter> FunctionParameter::parse(Lexer &lexer) {
  std::unique_ptr<FunctionParameter> parameter = std::make_unique<FunctionParameter>();
  parameter->start_parsing(lexer, "<function-parameter>");

  parameter->name = lexer.skip_name();
  lexer.skip({Token::Type::COLON});
  parameter->type = Type::parse(lexer);

  parameter->finish_parsing(lexer, "</function-parameter>");
  return parameter;
}

llvm::Value *FunctionParameter::compile(Compiler &compiler) const { unreachable_error(); }

void FunctionConstructor::parse_type(Lexer &lexer) {
  lexer.skip({Token::Type::FN});
  lexer.skip({Token::Type::LPAREN});
  lexer.skip({Token::Type::LPAREN});

  // TODO: do this in some constructor?
  this->type = std::make_unique<FunctionType>();

  while (lexer.peek()->type != Token::Type::RPAREN) {
    std::unique_ptr<FunctionParameter> parameter = FunctionParameter::parse(lexer);
    this->type->parameter_types.push_back(parameter->type);
    this->parameters.push_back(std::move(parameter));

    lexer.expect({Token::Type::COMMA, Token::Type::RPAREN});
    lexer.try_skip({Token::Type::COMMA});
  }

  lexer.skip({Token::Type::RPAREN});
}

void FunctionConstructor::parse_body(Lexer &lexer) {
  lexer.skip({Token::Type::FAT_RARROW});
  this->body = Constructor::parse(lexer);
  this->type->return_type = body->get_type();

  lexer.skip({Token::Type::RPAREN});
}

std::unique_ptr<FunctionConstructor> FunctionConstructor::parse(Lexer &lexer) {
  std::unique_ptr<FunctionConstructor> constructor = std::make_unique<FunctionConstructor>();
  constructor->start_parsing(lexer, "<function-constructor>");

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  constructor->finish_parsing(lexer, "</function-constructor>");
  return constructor;
}

llvm::Value *FunctionConstructor::compile(Compiler &compiler) const {
  // NOTE: parameters of function are inferred by the prototype, however we still have to set their
  // names which is done by FunctionParameter::compile
  llvm::FunctionType *prototype = this->type->get_llvm_type(compiler);
  llvm::Function *function =
      compiler.define_function(prototype, this->name, *this->body, this->parameters);

  return function;
}

void PrimitiveConstructor::parse_type(Lexer &lexer) { this->type = PrimitiveType::parse(lexer); }

void PrimitiveConstructor::parse_body(Lexer &lexer) {
  lexer.skip({Token::Type::LPAREN});
  while (lexer.peek()->type != Token::Type::RPAREN)
    this->body.push_back(Statement::parse(lexer));

  if (!this->body.back()->is_expression())
    this->parser_error("missing return, last statement in each function must be an expression",
                       lexer);

  lexer.skip({Token::Type::RPAREN});
}

std::unique_ptr<PrimitiveConstructor> PrimitiveConstructor::parse(Lexer &lexer) {
  std::unique_ptr<PrimitiveConstructor> constructor = std::make_unique<PrimitiveConstructor>();
  constructor->start_parsing(lexer, "<primitive-constructor>");

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  constructor->finish_parsing(lexer, "</primitive-constructor>");
  return constructor;
}

llvm::Value *PrimitiveConstructor::compile(Compiler &compiler) const {
  compiler.start_scope();
  for (size_t i = 0; i < this->body.size() - 1; ++i)
    this->body[i]->compile(compiler);

  // Return value of each constructor is the last statement (which is an expression)
  llvm::Value *return_value = this->body.back()->compile(compiler);
  compiler.end_scope();

  return return_value;
}

} // namespace Kebab::Parser
