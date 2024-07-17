#include "parser/Type.hpp"
#include "lexer/Lexer.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

namespace Kebab {
namespace Parser {

std::unique_ptr<Type> Type::parse(Lexer &lexer) {
  std::unique_ptr<Type> type;

  switch (lexer.peek()->type) {
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
    parser_error(std::string("reached unreachable branch with token: ") +
                     lexer.peek()->to_string_short(),
                 lexer);
  }

  return type;
}

std::unique_ptr<ListType> ListType::parse(Lexer &lexer) {
  std::unique_ptr<ListType> type = std::make_unique<ListType>();
  type->start_parsing(lexer, "<list-type>");

  lexer.skip({Token::Type::LIST});
  lexer.skip({Token::Type::LPAREN});
  type->content_type = Type::parse(lexer);
  lexer.skip({Token::Type::RPAREN});

  type->finish_parsing(lexer, "</list-type>");
  return type;
}

llvm::ScalableVectorType *ListType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  // TODO:
  assert(false && "unimplemented function ListType::get_llvm_type");
}

llvm::Value *ListType::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function ListType::compile");
}

void FunctionType::parse_parameter_types(Lexer &lexer) {
  while (lexer.peek()->type != Token::Type::RPAREN) {
    this->parameter_types.push_back(Type::parse(lexer));

    lexer.expect({Token::Type::COMMA, Token::Type::RPAREN});
    lexer.try_skip({Token::Type::COMMA});
  }
}

void FunctionType::parse_return_type(Lexer &lexer) { this->return_type = Type::parse(lexer); }

std::unique_ptr<FunctionType> FunctionType::parse(Lexer &lexer) {
  std::unique_ptr<FunctionType> type = std::make_unique<FunctionType>();
  type->start_parsing(lexer, "<function-type>");

  lexer.skip({Token::Type::FN});
  lexer.skip({Token::Type::LPAREN});

  type->parse_parameter_types(lexer);

  lexer.skip({Token::Type::RPAREN});
  lexer.skip({Token::Type::FAT_RARROW});

  type->parse_return_type(lexer);

  type->finish_parsing(lexer, "</function-type>");
  return type;
}

llvm::FunctionType *FunctionType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  llvm::Type *llvm_return_type = this->return_type->get_llvm_type(builder);
  std::vector<llvm::Type *> llvm_parameter_types;
  for (const auto &parameter_type : this->parameter_types)
    llvm_parameter_types.push_back(parameter_type->get_llvm_type(builder));

  // TODO: varargs
  return llvm::FunctionType::get(llvm_return_type, llvm_parameter_types, false);
}

llvm::Value *FunctionType::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function FunctionType::compile");
}

std::unique_ptr<PrimitiveType> PrimitiveType::parse(Lexer &lexer) {
  std::unique_ptr<PrimitiveType> type = std::make_unique<PrimitiveType>();
  type->start_parsing(lexer, "<primitive-type>");

  type->name = lexer.skip_name();

  type->finish_parsing(lexer, "</primitive-type>");
  return type;
}

llvm::Type *PrimitiveType::get_llvm_type(llvm::IRBuilder<> &builder) const {
  if (this->name.compare("int") == 0)
    return builder.getInt64Ty();
  else if (this->name.compare("float") == 0)
    return builder.getDoubleTy();
  else if (this->name.compare("char") == 0)
    return builder.getInt8Ty();
  else if (this->name.compare("string") == 0)
    return builder.getInt8Ty()->getPointerTo();
  else if (this->name.compare("bool") == 0)
    return builder.getInt1Ty();
  else if (this->name.compare("void") == 0)
    return builder.getVoidTy();

  this->unrecognized_type_error(this->name);
}

llvm::Value *PrimitiveType::compile(Compiler &compiler) const {
  // TODO:
  assert(false && "unimplemented function PrimitiveType::compile");
}

} // namespace Parser
} // namespace Kebab
