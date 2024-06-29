#include <cassert>
#include <memory>

#include "Constructor.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<Constructor> Constructor::parse(Lexer &lexer) {
  start_parsing("constructor");
  std::unique_ptr<Constructor> constructor;

  switch (lexer.cur_token.kind) {
  case Token::Kind::FN:
    constructor = FunctionConstructor::parse(lexer);
    break;

  case Token::Kind::LIST:
    constructor = ListConstructor::parse(lexer);
    break;

  case Token::Kind::NAME:
    constructor = PrimitiveConstructor::parse(lexer);
    break;

  default:
    error("unexpected token: '" + lexer.cur_token.to_string() + "' expected some type");
  }

  end_parsing("constructor");
  return constructor;
}

void ListConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Kind::LIST);
  skip(lexer, Token::Kind::LPAREN);

  skip(lexer, Token::Kind::LPAREN);
  this->type = std::make_unique<ListType>(); // TODO: make in constructor somewhere?
  this->type->content_type = Type::parse(lexer);
  skip(lexer, Token::Kind::RPAREN);
}

void ListConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Kind::FAT_RARROW);
  while (lexer.cur_token.kind != Token::Kind::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Kind::RPAREN);
}

std::unique_ptr<ListConstructor> ListConstructor::parse(Lexer &lexer) {
  start_parsing("list-constructor");
  std::unique_ptr<ListConstructor> constructor = std::make_unique<ListConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  end_parsing("list-constructor");
  return constructor;
}

std::unique_ptr<FunctionParameter> FunctionParameter::parse(Lexer &lexer) {
  start_parsing("function-parameter");
  std::unique_ptr<FunctionParameter> parameter = std::make_unique<FunctionParameter>();

  parameter->name = skip_name(lexer);
  skip(lexer, Token::Kind::COLON);
  parameter->type = Type::parse(lexer);

  end_parsing("function-parameter");
  return parameter;
}

void FunctionConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Kind::FN);
  skip(lexer, Token::Kind::LPAREN);
  skip(lexer, Token::Kind::LPAREN);

  // TODO: do this in some constructor?
  this->type = std::make_unique<FunctionType>();

  while (lexer.cur_token.kind != Token::Kind::RPAREN) {
    std::unique_ptr<FunctionParameter> parameter = FunctionParameter::parse(lexer);
    this->type->parameter_types.push_back(parameter->type);
    this->parameters.push_back(std::move(parameter));

    expect(lexer, Token::Kind::COMMA, Token::Kind::RPAREN);
    ignore(lexer, Token::Kind::COMMA);
  }

  skip(lexer, Token::Kind::RPAREN);
}

void FunctionConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Kind::FAT_RARROW);
  std::unique_ptr<Constructor> body = Constructor::parse(lexer);
  this->type->return_type = body->type;

  skip(lexer, Token::Kind::RPAREN);
}

std::unique_ptr<FunctionConstructor> FunctionConstructor::parse(Lexer &lexer) {
  start_parsing("function-constructor");
  std::unique_ptr<FunctionConstructor> constructor = std::make_unique<FunctionConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  end_parsing("function-constructor");
  return constructor;
}

void PrimitiveConstructor::parse_type(Lexer &lexer) { this->type = PrimitiveType::parse(lexer); }

void PrimitiveConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Kind::LPAREN);
  while (lexer.cur_token.kind != Token::Kind::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Kind::RPAREN);
}

std::unique_ptr<PrimitiveConstructor> PrimitiveConstructor::parse(Lexer &lexer) {
  start_parsing("primitive-constructor");
  std::unique_ptr<PrimitiveConstructor> constructor = std::make_unique<PrimitiveConstructor>();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  end_parsing("primitive-constructor");
  return constructor;
}

} // namespace Parser
} // namespace Kebab
