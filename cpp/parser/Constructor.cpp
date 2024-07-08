#include <cassert>
#include <memory>

#include "lexer/Lexer.hpp"
#include "logging/Logger.hpp"
#include "parser/Constructor.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<Constructor> Constructor::parse(Lexer &lexer) {
  Logger::log_with_indent("<constructor>");
  std::unique_ptr<Constructor> constructor;

  switch (lexer.cur_token->type) {
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
    error("unexpected token '" + lexer.cur_token->to_string_short() + "' expected some type",
          lexer);
  }

  Logger::log_with_dedent("<constructor/>");
  return constructor;
}

void ListConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Type::LIST);
  skip(lexer, Token::Type::LPAREN);

  skip(lexer, Token::Type::LPAREN);
  this->type = std::make_unique<ListType>(); // TODO: make in constructor somewhere?
  this->type->content_type = Type::parse(lexer);
  skip(lexer, Token::Type::RPAREN);
}

void ListConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Type::FAT_RARROW);
  while (lexer.cur_token->type != Token::Type::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Type::RPAREN);
}

std::unique_ptr<ListConstructor> ListConstructor::parse(Lexer &lexer) {
  Logger::log_with_indent("<list-constructor>");
  std::unique_ptr<ListConstructor> constructor = std::make_unique<ListConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  Logger::log_with_dedent("<list-constructor/>");
  return constructor;
}

llvm::Value *ListConstructor::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function ListConstructor::compile");
}

std::unique_ptr<FunctionParameter> FunctionParameter::parse(Lexer &lexer) {
  Logger::log_with_indent("<function-parameter>");
  std::unique_ptr<FunctionParameter> parameter = std::make_unique<FunctionParameter>();

  parameter->name = skip_name(lexer);
  skip(lexer, Token::Type::COLON);
  parameter->type = Type::parse(lexer);

  Logger::log_with_dedent("<function-parameter/>");
  return parameter;
}

llvm::Value *FunctionParameter::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionParameter::compile");
}

void FunctionConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Type::FN);
  skip(lexer, Token::Type::LPAREN);
  skip(lexer, Token::Type::LPAREN);

  // TODO: do this in some constructor?
  this->type = std::make_unique<FunctionType>();

  while (lexer.cur_token->type != Token::Type::RPAREN) {
    std::unique_ptr<FunctionParameter> parameter = FunctionParameter::parse(lexer);
    this->type->parameter_types.push_back(parameter->type);
    this->parameters.push_back(std::move(parameter));

    expect(lexer, Token::Type::COMMA, Token::Type::RPAREN);
    ignore(lexer, Token::Type::COMMA);
  }

  skip(lexer, Token::Type::RPAREN);
}

void FunctionConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Type::FAT_RARROW);
  std::unique_ptr<Constructor> body = Constructor::parse(lexer);
  this->type->return_type = body->get_type();

  skip(lexer, Token::Type::RPAREN);
}

std::unique_ptr<FunctionConstructor> FunctionConstructor::parse(Lexer &lexer) {
  Logger::log_with_indent("<function-constructor>");
  std::unique_ptr<FunctionConstructor> constructor = std::make_unique<FunctionConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  Logger::log_with_dedent("<function-constructor/>");
  return constructor;
}

llvm::Value *FunctionConstructor::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionConstructor::compile");
}

void PrimitiveConstructor::parse_type(Lexer &lexer) { this->type = PrimitiveType::parse(lexer); }

void PrimitiveConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Type::LPAREN);
  while (lexer.cur_token->type != Token::Type::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Type::RPAREN);
}

std::unique_ptr<PrimitiveConstructor> PrimitiveConstructor::parse(Lexer &lexer) {
  Logger::log_with_indent("<primitive-constructor>");
  std::unique_ptr<PrimitiveConstructor> constructor = std::make_unique<PrimitiveConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  Logger::log_with_dedent("<primitive-constructor/>");
  return constructor;
}

llvm::Value *PrimitiveConstructor::compile(Compiler &compiler) const {
  for (size_t i = 0; i < this->body.size() - 1; ++i)
    this->body[i]->compile(compiler);

  return this->body.back()->compile(compiler);
}

} // namespace Parser
} // namespace Kebab
