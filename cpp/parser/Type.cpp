#include "Type.hpp"
#include "lexer/Lexer.hpp"
#include <cassert>

namespace Kebab {
namespace Parser {

std::unique_ptr<Type> Type::parse(Lexer &lexer) {
  start_parsing("type");
  std::unique_ptr<Type> type;

  switch (lexer.cur_token.kind) {
  case Token::Kind::FN:
    type = FunctionType::parse(lexer);
    break;

  case Token::Kind::LIST:
    type = ListType::parse(lexer);
    break;

  case Token::Kind::NAME:
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

  skip(lexer, Token::Kind::LIST);
  skip(lexer, Token::Kind::LPAREN);
  type->content_type = Type::parse(lexer);
  skip(lexer, Token::Kind::RPAREN);

  end_parsing("list-type");
  return type;
}

void FunctionType::parse_parameter_types(Lexer &lexer) {
  while (lexer.cur_token.kind != Token::Kind::RPAREN) {
    this->parameter_types.push_back(Type::parse(lexer));

    expect(lexer, Token::Kind::COMMA, Token::Kind::RPAREN);
    ignore(lexer, Token::Kind::COMMA);
  }
}

void FunctionType::parse_return_type(Lexer &lexer) { this->return_type = Type::parse(lexer); }

std::unique_ptr<FunctionType> FunctionType::parse(Lexer &lexer) {
  start_parsing("function-type");
  std::unique_ptr<FunctionType> type = std::make_unique<FunctionType>();

  skip(lexer, Token::Kind::FN);
  skip(lexer, Token::Kind::LPAREN);

  type->parse_parameter_types(lexer);

  skip(lexer, Token::Kind::RPAREN);
  skip(lexer, Token::Kind::FAT_RARROW);

  type->parse_return_type(lexer);

  end_parsing("function-type");
  return type;
}

std::unique_ptr<PrimitiveType> PrimitiveType::parse(Lexer &lexer) {
  start_parsing("primitive-type");
  std::unique_ptr<PrimitiveType> type = std::make_unique<PrimitiveType>();

  type->name = skip_name(lexer);

  end_parsing("primitive-type");
  return type;
}

} // namespace Parser
} // namespace Kebab
