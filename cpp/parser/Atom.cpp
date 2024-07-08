#include <cassert>

#include "logging/Logger.hpp"
#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"

namespace Kebab {
namespace Parser {

std::unique_ptr<IntAtom> IntAtom::parse(Lexer &lexer) {
  std::unique_ptr<IntAtom> atom = std::make_unique<IntAtom>();
  atom->start_parsing(lexer, "<int-atom>");

  atom->i = skip_int(lexer);

  atom->finish_parsing(lexer, "</int-atom>");
  return atom;
}

llvm::Value *IntAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt64Ty(), this->i);
}

std::unique_ptr<FloatAtom> FloatAtom::parse(Lexer &lexer) {
  std::unique_ptr<FloatAtom> atom = std::make_unique<FloatAtom>();
  atom->start_parsing(lexer, "<float-atom>");

  atom->f = skip_float(lexer);

  atom->finish_parsing(lexer, "</float-atom>");
  return atom;
}

llvm::Value *FloatAtom::compile(Compiler &compiler) const {
  return llvm::ConstantFP::get(compiler.builder.getFloatTy(), this->f);
}

std::unique_ptr<CharAtom> CharAtom::parse(Lexer &lexer) {
  std::unique_ptr<CharAtom> atom = std::make_unique<CharAtom>();
  atom->start_parsing(lexer, "<char-atom>");

  atom->c = skip_char(lexer);

  atom->finish_parsing(lexer, "</char-atom>");
  return atom;
}

llvm::Value *CharAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt8Ty(), this->c);
}

std::unique_ptr<StringAtom> StringAtom::parse(Lexer &lexer) {
  std::unique_ptr<StringAtom> atom = std::make_unique<StringAtom>();
  atom->start_parsing(lexer, "<string-atom>");

  atom->s = skip_string(lexer);

  atom->finish_parsing(lexer, "</string-atom>");
  return atom;
}

llvm::Value *StringAtom::compile(Compiler &compiler) const {
  return compiler.builder.CreateGlobalStringPtr(this->s);
}

std::unique_ptr<BoolAtom> BoolAtom::parse(Lexer &lexer) {
  std::unique_ptr<BoolAtom> atom = std::make_unique<BoolAtom>();
  atom->start_parsing(lexer, "<bool-atom>");

  if (lexer.cur_token->type == Token::Type::TRUE)
    atom->b = true;
  else if (lexer.cur_token->type == Token::Type::FALSE)
    atom->b = false;
  lexer.advance();

  atom->finish_parsing(lexer, "</bool-atom>");
  return atom;
}

llvm::Value *BoolAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt1Ty(), this->b);
}

std::unique_ptr<NameAtom> NameAtom::parse(Lexer &lexer) {
  std::unique_ptr<NameAtom> atom = std::make_unique<NameAtom>();
  atom->start_parsing(lexer, "<name-atom>");

  atom->name = skip_name(lexer);

  atom->finish_parsing(lexer, "</name-atom>");
  return atom;
}

llvm::Value *NameAtom::compile(Compiler &compiler) const {
  llvm::GlobalValue *names_global = compiler.module.getNamedValue(this->name);
  if (names_global == nullptr)
    compiler.error(std::string("undefined identifier: '") + this->name + '\'');

  return names_global;
}

std::unique_ptr<InnerExpressionAtom> InnerExpressionAtom::parse(Lexer &lexer) {
  std::unique_ptr<InnerExpressionAtom> atom = std::make_unique<InnerExpressionAtom>();
  atom->start_parsing(lexer, "<inner-expression-atom>");

  skip(lexer, Token::Type::LPAREN);
  atom->expression = Expression::parse(lexer);
  skip(lexer, Token::Type::RPAREN);

  atom->finish_parsing(lexer, "</inner-expression-atom>");
  return atom;
}

llvm::Value *InnerExpressionAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function InnerExpressionAtom::compile");
}

std::unique_ptr<ListAtom> ListAtom::parse(Lexer &lexer) {
  std::unique_ptr<ListAtom> atom = std::make_unique<ListAtom>();
  atom->start_parsing(lexer, "<list-atom>");

  skip(lexer, Token::Type::LBRACKET);
  while (lexer.cur_token->type != Token::Type::RBRACKET) {
    atom->list.push_back(Expression::parse(lexer));

    expect(lexer, Token::Type::COMMA, Token::Type::RBRACKET);
    ignore(lexer, Token::Type::COMMA);
  }
  skip(lexer, Token::Type::RBRACKET);

  atom->finish_parsing(lexer, "</list-atom>");
  return atom;
}

llvm::Value *ListAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function ListAtom::compile");
}

std::unique_ptr<Atom> Atom::parse(Lexer &lexer) {
  std::unique_ptr<Atom> atom;

  switch (lexer.cur_token->type) {
  case Token::Type::INT_LITERAL:
    atom = IntAtom::parse(lexer);
    break;

  case Token::Type::FLOAT_LITERAL:
    atom = FloatAtom::parse(lexer);
    break;

  case Token::Type::CHAR_LITERAL:
    atom = CharAtom::parse(lexer);
    break;

  case Token::Type::STRING_LITERAL:
    atom = StringAtom::parse(lexer);
    break;

  case Token::Type::TRUE:
  case Token::Type::FALSE:
    atom = BoolAtom::parse(lexer);
    break;

  case Token::Type::NAME:
    atom = NameAtom::parse(lexer);
    break;

  case Token::Type::LPAREN:
    atom = InnerExpressionAtom::parse(lexer);
    break;

  case Token::Type::LBRACKET:
    atom = ListAtom::parse(lexer);
    break;

  default:
    parser_error(std::string("reached unreachable branch with token '") +
                     lexer.cur_token->to_string_short() + '\'',
                 lexer);
  }

  return atom;
}

} // namespace Parser
} // namespace Kebab
