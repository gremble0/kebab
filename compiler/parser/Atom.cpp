#include <algorithm>
#include <cassert>
#include <variant>

#include "parser/Atom.hpp"
#include "parser/Expression.hpp"
#include "parser/Statement.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

namespace Kebab::Parser {

std::unique_ptr<IntAtom> IntAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<IntAtom>();
  atom->start_parsing(lexer, "<int-atom>");

  atom->i = lexer.skip_int();

  atom->finish_parsing(lexer, "</int-atom>");
  return atom;
}

llvm::Value *IntAtom::compile(Compiler &compiler) const { return compiler.create_int(this->i); }

std::unique_ptr<FloatAtom> FloatAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<FloatAtom>();
  atom->start_parsing(lexer, "<float-atom>");

  atom->f = lexer.skip_float();

  atom->finish_parsing(lexer, "</float-atom>");
  return atom;
}

llvm::Value *FloatAtom::compile(Compiler &compiler) const { return compiler.create_float(this->f); }

std::unique_ptr<CharAtom> CharAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<CharAtom>();
  atom->start_parsing(lexer, "<char-atom>");

  atom->c = lexer.skip_char();

  atom->finish_parsing(lexer, "</char-atom>");
  return atom;
}

llvm::Value *CharAtom::compile(Compiler &compiler) const { return compiler.create_char(this->c); }

std::unique_ptr<StringAtom> StringAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<StringAtom>();
  atom->start_parsing(lexer, "<string-atom>");

  atom->s = lexer.skip_string();

  atom->finish_parsing(lexer, "</string-atom>");
  return atom;
}

llvm::Value *StringAtom::compile(Compiler &compiler) const {
  return compiler.create_string(this->s);
}

std::unique_ptr<BoolAtom> BoolAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<BoolAtom>();
  atom->start_parsing(lexer, "<bool-atom>");

  if (lexer.peek()->type == Token::Type::TRUE)
    atom->b = true;
  else if (lexer.peek()->type == Token::Type::FALSE)
    atom->b = false;
  lexer.advance();

  atom->finish_parsing(lexer, "</bool-atom>");
  return atom;
}

llvm::Value *BoolAtom::compile(Compiler &compiler) const { return compiler.create_bool(this->b); }

std::unique_ptr<NameAtom> NameAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<NameAtom>();
  atom->start_parsing(lexer, "<name-atom>");

  atom->name = lexer.skip_name();

  atom->finish_parsing(lexer, "</name-atom>");
  return atom;
}

llvm::Value *NameAtom::compile(Compiler &compiler) const {
  auto value = compiler.get_value(this->name);
  if (std::holds_alternative<llvm::Value *>(value))
    return std::get<llvm::Value *>(value);
  else
    this->compiler_error(std::get<NameError>(value));
}

std::unique_ptr<InnerExpressionAtom> InnerExpressionAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<InnerExpressionAtom>();
  atom->start_parsing(lexer, "<inner-expression-atom>");

  lexer.skip({Token::Type::LPAREN});
  atom->expression = Expression::parse(lexer);
  lexer.skip({Token::Type::RPAREN});

  atom->finish_parsing(lexer, "</inner-expression-atom>");
  return atom;
}

llvm::Value *InnerExpressionAtom::compile(Compiler &compiler) const {
  return this->expression->compile(compiler);
}

std::unique_ptr<ListAtom> ListAtom::parse(Lexer &lexer) {
  auto atom = std::make_unique<ListAtom>();
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
  std::vector<llvm::Value *> elements_compiled;
  for (const std::unique_ptr<Expression> &element : this->list)
    elements_compiled.push_back(element->compile(compiler));

  // Type check that the list is homogenous
  llvm::Type *expected_type = elements_compiled.front()->getType();
  std::ranges::for_each(elements_compiled, [this, expected_type](const llvm::Value *v) {
    const llvm::Type *actual_type = v->getType();
    if (auto error = NonhomogenousListError::check(expected_type, actual_type); error.has_value())
      this->compiler_error(error.value());
  });

  return compiler.create_list(elements_compiled, expected_type);
}

std::unique_ptr<Atom> Atom::parse(Lexer &lexer) {
  std::unique_ptr<Atom> atom;

  switch (lexer.peek()->type) {
    using enum Token::Type;
  case INT_LITERAL:
    atom = IntAtom::parse(lexer);
    break;

  case FLOAT_LITERAL:
    atom = FloatAtom::parse(lexer);
    break;

  case CHAR_LITERAL:
    atom = CharAtom::parse(lexer);
    break;

  case STRING_LITERAL:
    atom = StringAtom::parse(lexer);
    break;

  case TRUE:
  case FALSE:
    atom = BoolAtom::parse(lexer);
    break;

  case NAME:
    atom = NameAtom::parse(lexer);
    break;

  case LPAREN:
    atom = InnerExpressionAtom::parse(lexer);
    break;

  case LBRACKET:
    atom = ListAtom::parse(lexer);
    break;

  default:
    // TODO: these unreachables are not good
    AstNode::parser_error(std::string("reached unreachable branch with token '") +
                              lexer.peek()->to_string_short() + '\'',
                          lexer);
  }

  return atom;
}

} // namespace Kebab::Parser
