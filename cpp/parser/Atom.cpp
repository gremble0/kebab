#include <cassert>

#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"

namespace Kebab {
namespace Parser {

std::unique_ptr<IntAtom> IntAtom::parse(Lexer &lexer) {
  std::unique_ptr<IntAtom> atom = std::make_unique<IntAtom>();
  atom->start_parsing(lexer, "<int-atom>");

  atom->i = lexer.skip_int();

  atom->finish_parsing(lexer, "</int-atom>");
  return atom;
}

llvm::Value *IntAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt64Ty(), this->i);
}

std::unique_ptr<FloatAtom> FloatAtom::parse(Lexer &lexer) {
  std::unique_ptr<FloatAtom> atom = std::make_unique<FloatAtom>();
  atom->start_parsing(lexer, "<float-atom>");

  atom->f = lexer.skip_float();

  atom->finish_parsing(lexer, "</float-atom>");
  return atom;
}

llvm::Value *FloatAtom::compile(Compiler &compiler) const {
  return llvm::ConstantFP::get(compiler.builder.getDoubleTy(), this->f);
}

std::unique_ptr<CharAtom> CharAtom::parse(Lexer &lexer) {
  std::unique_ptr<CharAtom> atom = std::make_unique<CharAtom>();
  atom->start_parsing(lexer, "<char-atom>");

  atom->c = lexer.skip_char();

  atom->finish_parsing(lexer, "</char-atom>");
  return atom;
}

llvm::Value *CharAtom::compile(Compiler &compiler) const {
  return llvm::ConstantInt::get(compiler.builder.getInt8Ty(), this->c);
}

std::unique_ptr<StringAtom> StringAtom::parse(Lexer &lexer) {
  std::unique_ptr<StringAtom> atom = std::make_unique<StringAtom>();
  atom->start_parsing(lexer, "<string-atom>");

  atom->s = lexer.skip_string();

  atom->finish_parsing(lexer, "</string-atom>");
  return atom;
}

llvm::Value *StringAtom::compile(Compiler &compiler) const {
  return compiler.builder.CreateGlobalStringPtr(this->s);
}

std::unique_ptr<BoolAtom> BoolAtom::parse(Lexer &lexer) {
  std::unique_ptr<BoolAtom> atom = std::make_unique<BoolAtom>();
  atom->start_parsing(lexer, "<bool-atom>");

  // TODO: make bool token contain inner bool to not have to do this
  if (lexer.peek()->type == Token::Type::TRUE)
    atom->b = true;
  else if (lexer.peek()->type == Token::Type::FALSE)
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

  atom->name = lexer.skip_name();

  atom->finish_parsing(lexer, "</name-atom>");
  return atom;
}

llvm::Value *NameAtom::compile(Compiler &compiler) const {
  std::optional<llvm::Value *> variable = compiler.get_value(this->name);
  if (variable.has_value())
    return variable.value();
  else
    compiler.error(std::string("undeclared identifier: '") + this->name + '\'');
}

std::unique_ptr<InnerExpressionAtom> InnerExpressionAtom::parse(Lexer &lexer) {
  std::unique_ptr<InnerExpressionAtom> atom = std::make_unique<InnerExpressionAtom>();
  atom->start_parsing(lexer, "<inner-expression-atom>");

  lexer.skip({Token::Type::LPAREN});
  atom->expression = Expression::parse(lexer);
  lexer.skip({Token::Type::RPAREN});

  atom->finish_parsing(lexer, "</inner-expression-atom>");
  return atom;
}

llvm::Value *InnerExpressionAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function InnerExpressionAtom::compile");
}

std::unique_ptr<ListAtom> ListAtom::parse(Lexer &lexer) {
  std::unique_ptr<ListAtom> atom = std::make_unique<ListAtom>();
  atom->start_parsing(lexer, "<list-atom>");

  lexer.skip({Token::Type::LBRACKET});
  while (lexer.peek()->type != Token::Type::RBRACKET) {
    atom->list.push_back(Expression::parse(lexer));

    lexer.expect({Token::Type::COMMA, Token::Type::RBRACKET});
    lexer.try_skip({Token::Type::COMMA});
  }
  lexer.skip({Token::Type::RBRACKET});

  atom->finish_parsing(lexer, "</list-atom>");
  return atom;
}

llvm::Value *ListAtom::compile(Compiler &compiler) const {
  assert(false && "unimplemented function ListAtom::compile");
}

std::unique_ptr<Atom> Atom::parse(Lexer &lexer) {
  std::unique_ptr<Atom> atom;

  switch (lexer.peek()->type) {
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
    // TODO: these unreachables are not good
    parser_error(std::string("reached unreachable branch with token '") +
                     lexer.peek()->to_string_short() + '\'',
                 lexer);
  }

  return atom;
}

} // namespace Parser
} // namespace Kebab
