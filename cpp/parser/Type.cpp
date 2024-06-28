#include "Type.hpp"
#include <cassert>

namespace Kebab {
namespace Parser {

Type *Type::parse(Lexer &lexer) {
  start_parsing("type");
  Type *type;

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
    // Unreachable
    assert(false);
  }

  end_parsing("type");
  return type;
}

ListType *ListType::parse(Lexer &lexer) {
  start_parsing("list-type");
  end_parsing("list-type");
}

FunctionType *FunctionType::parse(Lexer &lexer) {
  start_parsing("function-type");
  end_parsing("function-type");
}

PrimitiveType *PrimitiveType::parse(Lexer &lexer) {
  start_parsing("primitive-type");
  end_parsing("primitive-type");
}

} // namespace Parser
} // namespace Kebab
