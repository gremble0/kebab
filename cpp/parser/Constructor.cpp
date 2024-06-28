#include <cassert>

#include "Constructor.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

Constructor *Constructor::parse(Lexer &lexer) {
  start_parsing("constructor");
  Constructor *constructor;

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
    assert(false && "Unreachable");
  }

  end_parsing("constructor");
  return constructor;
}

void ListConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Kind::LIST);
  skip(lexer, Token::Kind::LPAREN);

  skip(lexer, Token::Kind::LPAREN);
  this->type = ListType::parse(lexer);
  skip(lexer, Token::Kind::RPAREN);
}

void ListConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Kind::FAT_RARROW);
  while (lexer.cur_token.kind != Token::Kind::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Kind::RPAREN);
}

ListConstructor *ListConstructor::parse(Lexer &lexer) {
  start_parsing("list-constructor");
  ListConstructor *constructor = new ListConstructor();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  end_parsing("list-constructor");
  return constructor;
}

FunctionParameter *FunctionParameter::parse(Lexer &lexer) {
  start_parsing("function-parameter");
  FunctionParameter *parameter = new FunctionParameter();

  expect(lexer, Token::Kind::NAME);
  parameter->name = std::get<std::string>(lexer.cur_token.value);
  lexer.advance();

  skip(lexer, Token::Kind::COLON);

  parameter->type = Type::parse(lexer);

  end_parsing("function-parameter");
  return parameter;
}

void FunctionConstructor::parse_type(Lexer &lexer) {
  skip(lexer, Token::Kind::FN);
  skip(lexer, Token::Kind::LPAREN);

  while (true) {
    FunctionParameter *parameter = FunctionParameter::parse(lexer);
    this->type->parameter_types.push_back(parameter->type);
    this->parameters.push_back(parameter);

    if (lexer.cur_token.kind == Token::Kind::RPAREN)
      break;
    else
      skip(lexer, Token::Kind::COMMA);
  }

  skip(lexer, Token::Kind::RPAREN);
}

void FunctionConstructor::parse_body(Lexer &lexer) {
  skip(lexer, Token::Kind::FAT_RARROW);
  while (lexer.cur_token.kind != Token::Kind::RPAREN)
    this->body.push_back(Statement::parse(lexer));
  skip(lexer, Token::Kind::RPAREN);
}

FunctionConstructor *FunctionConstructor::parse(Lexer &lexer) {
  start_parsing("function-constructor");
  FunctionConstructor *constructor = new FunctionConstructor();

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

PrimitiveConstructor *PrimitiveConstructor::parse(Lexer &lexer) {
  start_parsing("primitive-constructor");
  PrimitiveConstructor *constructor = new PrimitiveConstructor();

  constructor->parse_type(lexer);
  constructor->parse_body(lexer);

  end_parsing("primitive-constructor");
  return constructor;
}

} // namespace Parser
} // namespace Kebab
