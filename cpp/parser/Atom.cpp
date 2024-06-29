#include "Atom.hpp"
#include "Expression.hpp"

namespace Kebab {
namespace Parser {

std::unique_ptr<IntAtom> IntAtom::parse(Lexer &lexer) {
  start_parsing("int-atom");
  std::unique_ptr<IntAtom> atom = std::make_unique<IntAtom>();

  atom->i = skip_int(lexer);

  end_parsing("int-atom");
  return atom;
}

std::unique_ptr<FloatAtom> FloatAtom::parse(Lexer &lexer) {
  start_parsing("float-atom");
  std::unique_ptr<FloatAtom> atom = std::make_unique<FloatAtom>();

  atom->f = skip_float(lexer);

  end_parsing("float-atom");
  return atom;
}

std::unique_ptr<CharAtom> CharAtom::parse(Lexer &lexer) {
  start_parsing("char-atom");
  std::unique_ptr<CharAtom> atom = std::make_unique<CharAtom>();

  atom->c = skip_char(lexer);

  end_parsing("char-atom");
  return atom;
}

std::unique_ptr<StringAtom> StringAtom::parse(Lexer &lexer) {
  start_parsing("string-atom");
  std::unique_ptr<StringAtom> atom = std::make_unique<StringAtom>();

  atom->s = skip_string(lexer);

  end_parsing("string-atom");
  return atom;
}

std::unique_ptr<BoolAtom> BoolAtom::parse(Lexer &lexer) {
  start_parsing("bool-atom");
  std::unique_ptr<BoolAtom> atom = std::make_unique<BoolAtom>();

  if (lexer.cur_token.type == Token::Type::TRUE)
    atom->b = true;
  else if (lexer.cur_token.type == Token::Type::FALSE)
    atom->b = false;
  lexer.advance();

  end_parsing("bool-atom");
  return atom;
}

std::unique_ptr<NameAtom> NameAtom::parse(Lexer &lexer) {
  start_parsing("name-atom");
  std::unique_ptr<NameAtom> atom = std::make_unique<NameAtom>();

  atom->name = skip_name(lexer);

  end_parsing("name-atom");
  return atom;
}

std::unique_ptr<InnerExpressionAtom> InnerExpressionAtom::parse(Lexer &lexer) {
  start_parsing("inner-expression-atom");
  std::unique_ptr<InnerExpressionAtom> atom = std::make_unique<InnerExpressionAtom>();

  skip(lexer, Token::Type::LPAREN);
  atom->expression = Expression::parse(lexer);
  skip(lexer, Token::Type::RPAREN);

  end_parsing("inner-expression-atom");
  return atom;
}

std::unique_ptr<ListAtom> ListAtom::parse(Lexer &lexer) {
  start_parsing("list-atom");
  std::unique_ptr<ListAtom> atom = std::make_unique<ListAtom>();

  skip(lexer, Token::Type::LBRACKET);
  while (lexer.cur_token.type != Token::Type::RBRACKET) {
    atom->list.push_back(Expression::parse(lexer));

    expect(lexer, Token::Type::COMMA, Token::Type::RBRACKET);
    ignore(lexer, Token::Type::COMMA);
  }
  skip(lexer, Token::Type::RBRACKET);

  end_parsing("list-atom");
  return atom;
}

std::unique_ptr<Atom> Atom::parse(Lexer &lexer) {
  start_parsing("atom");
  std::unique_ptr<Atom> atom;

  switch (lexer.cur_token.type) {
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
    error(std::string("reached unreachable branch with token: ") + lexer.cur_token.to_string());
  }

  end_parsing("atom");
  return atom;
}

} // namespace Parser
} // namespace Kebab
