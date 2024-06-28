#include "Type.hpp"

namespace Kebab {
namespace Parser {

Type *Type::parse(Lexer &lexer) {
  start_parsing("type");
  Type *type;

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
