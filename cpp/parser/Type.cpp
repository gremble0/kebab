#include "parser/Type.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<Type> Type::parse(Lexer &lexer) {
  start_parsing("type");
  std::unique_ptr<Type> type;

  switch (lexer.cur_token.type) {
  case Token::Type::FN:
    type = FunctionType::parse(lexer);
    break;

  case Token::Type::LIST:
    type = ListType::parse(lexer);
    break;

  case Token::Type::NAME:
    type = PrimitiveType::parse(lexer);
    break;

  default:
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  end_parsing("type");
  return type;
}

std::unique_ptr<ListType> ListType::parse(Lexer &lexer) {
  start_parsing("list-type");
  std::unique_ptr<ListType> type = std::make_unique<ListType>();

  skip(lexer, Token::Type::LIST);
  skip(lexer, Token::Type::LPAREN);
  type->content_type = Type::parse(lexer);
  skip(lexer, Token::Type::RPAREN);

  end_parsing("list-type");
  return type;
}

void ListType::compile(Compiler &compiler) {}

void FunctionType::parse_parameter_types(Lexer &lexer) {
  while (lexer.cur_token.type != Token::Type::RPAREN) {
    this->parameter_types.push_back(Type::parse(lexer));

    expect(lexer, Token::Type::COMMA, Token::Type::RPAREN);
    ignore(lexer, Token::Type::COMMA);
  }
}

void FunctionType::parse_return_type(Lexer &lexer) { this->return_type = Type::parse(lexer); }

std::unique_ptr<FunctionType> FunctionType::parse(Lexer &lexer) {
  start_parsing("function-type");
  std::unique_ptr<FunctionType> type = std::make_unique<FunctionType>();

  skip(lexer, Token::Type::FN);
  skip(lexer, Token::Type::LPAREN);

  type->parse_parameter_types(lexer);

  skip(lexer, Token::Type::RPAREN);
  skip(lexer, Token::Type::FAT_RARROW);

  type->parse_return_type(lexer);

  end_parsing("function-type");
  return type;
}

void FunctionType::compile(Compiler &compiler) {}

std::unique_ptr<PrimitiveType> PrimitiveType::parse(Lexer &lexer) {
  start_parsing("primitive-type");
  std::unique_ptr<PrimitiveType> type = std::make_unique<PrimitiveType>();

  type->name = skip_name(lexer);

  end_parsing("primitive-type");
  return type;
}

void PrimitiveType::compile(Compiler &compiler) {}

} // namespace Parser
} // namespace Kebab
