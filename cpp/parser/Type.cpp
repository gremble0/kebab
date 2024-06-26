#include "parser/Type.hpp"
#include "lexer/Lexer.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

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

llvm::ScalableVectorType *ListType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  // TODO:
  return nullptr;
}

void ListType::compile(Compiler::Compiler &compiler) const {
  // TODO:
}

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

llvm::FunctionType *FunctionType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  // TODO:
  return nullptr;
}

void FunctionType::compile(Compiler::Compiler &compiler) const {
  // TODO:
}

std::unique_ptr<PrimitiveType> PrimitiveType::parse(Lexer &lexer) {
  start_parsing("primitive-type");
  std::unique_ptr<PrimitiveType> type = std::make_unique<PrimitiveType>();

  type->name = skip_name(lexer);

  end_parsing("primitive-type");
  return type;
}

llvm::Type *PrimitiveType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  if (this->name.compare("int") == 0)
    return builder.getInt64Ty();
  else if (this->name.compare("float") == 0)
    return builder.getFloatTy();
  else if (this->name.compare("char") == 0)
    return builder.getInt8Ty();
  else if (this->name.compare("string") == 0)
    return builder.getInt8Ty()->getPointerTo();
  else if (this->name.compare("bool") == 0)
    return builder.getInt8Ty();

  error("unrecognized type: '" + this->name + '\'');
}

void PrimitiveType::compile(Compiler::Compiler &compiler) const {
  // TODO:
}

} // namespace Parser
} // namespace Kebab
