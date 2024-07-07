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
  Logger::log_with_indent("<int-atom>");
  std::unique_ptr<IntAtom> atom = std::make_unique<IntAtom>();

  atom->i = skip_int(lexer);

  Logger::log_with_dedent("<int-atom/>");
  return atom;
}

llvm::Value *IntAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt64Ty(), this->i);
}

std::unique_ptr<FloatAtom> FloatAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<float-atom>");
  std::unique_ptr<FloatAtom> atom = std::make_unique<FloatAtom>();

  atom->f = skip_float(lexer);

  Logger::log_with_dedent("<float-atom/>");
  return atom;
}

llvm::Value *FloatAtom::compile(Compiler &compiler) const {
  return llvm::ConstantFP::get(compiler.builder.getFloatTy(), this->f);
}

std::unique_ptr<CharAtom> CharAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<char-atom>");
  std::unique_ptr<CharAtom> atom = std::make_unique<CharAtom>();

  atom->c = skip_char(lexer);

  Logger::log_with_dedent("<char-atom/>");
  return atom;
}

llvm::Value *CharAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt8Ty(), this->c);
}

std::unique_ptr<StringAtom> StringAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<string-atom>");
  std::unique_ptr<StringAtom> atom = std::make_unique<StringAtom>();

  atom->s = skip_string(lexer);

  Logger::log_with_dedent("<string-atom/>");
  return atom;
}

llvm::Value *StringAtom::compile(Compiler &compiler) const {
  return compiler.builder.CreateGlobalStringPtr(this->s);
}

std::unique_ptr<BoolAtom> BoolAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<bool-atom>");
  std::unique_ptr<BoolAtom> atom = std::make_unique<BoolAtom>();

  if (lexer.cur_token->type == Token::Type::TRUE)
    atom->b = true;
  else if (lexer.cur_token->type == Token::Type::FALSE)
    atom->b = false;
  lexer.advance();

  Logger::log_with_dedent("<bool-atom/>");
  return atom;
}

llvm::Value *BoolAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt1Ty(), this->b);
}

std::unique_ptr<NameAtom> NameAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<name-atom>");
  std::unique_ptr<NameAtom> atom = std::make_unique<NameAtom>();

  atom->name = skip_name(lexer);

  Logger::log_with_dedent("<name-atom/>");
  return atom;
}

llvm::Value *NameAtom::compile(Compiler &compiler) const {
  llvm::GlobalValue *names_global = compiler.module.getNamedValue(this->name);
  if (names_global == nullptr)
    compiler.error(std::string("undefined identifier: '") + this->name + '\'');

  return names_global;
}

std::unique_ptr<InnerExpressionAtom> InnerExpressionAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<inner-expression-atom>");
  std::unique_ptr<InnerExpressionAtom> atom = std::make_unique<InnerExpressionAtom>();

  skip(lexer, Token::Type::LPAREN);
  atom->expression = Expression::parse(lexer);
  skip(lexer, Token::Type::RPAREN);

  Logger::log_with_dedent("<inner-expression-atom/>");
  return atom;
}

llvm::Value *InnerExpressionAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function InnerExpressionAtom::compile");
}

std::unique_ptr<ListAtom> ListAtom::parse(Lexer &lexer) {
  Logger::log_with_indent("<list-atom>");
  std::unique_ptr<ListAtom> atom = std::make_unique<ListAtom>();

  skip(lexer, Token::Type::LBRACKET);
  while (lexer.cur_token->type != Token::Type::RBRACKET) {
    atom->list.push_back(Expression::parse(lexer));

    expect(lexer, Token::Type::COMMA, Token::Type::RBRACKET);
    ignore(lexer, Token::Type::COMMA);
  }
  skip(lexer, Token::Type::RBRACKET);

  Logger::log_with_dedent("<list-atom/>");
  return atom;
}

llvm::Value *ListAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function ListAtom::compile");
}

std::unique_ptr<Atom> Atom::parse(Lexer &lexer) {
  Logger::log_with_indent("<atom>");
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
    error(std::string("reached unreachable branch with token: ") +
          lexer.cur_token->to_string_short());
  }

  Logger::log_with_dedent("<atom/>");
  return atom;
}

} // namespace Parser
} // namespace Kebab
