#include <cassert>
#include <cstdint>

#include "Atom.hpp"
#include "Expression.hpp"

CharAtom *CharAtom::parse(Lexer &lexer) {
  start_parsing("char-atom");
  CharAtom *atom = new CharAtom();

  expect(lexer, Token::Kind::CHAR_LITERAL);
  atom->c = std::get<uint8_t>(lexer.cur_token.value);
  lexer.advance();

  end_parsing("char-atom");
  return atom;
}

StringAtom *StringAtom::parse(Lexer &lexer) {
  start_parsing("string-atom");
  StringAtom *atom = new StringAtom();

  expect(lexer, Token::Kind::CHAR_LITERAL);
  atom->s = std::get<std::string>(lexer.cur_token.value);
  lexer.advance();

  end_parsing("string-atom");
  return atom;
}

BoolAtom *BoolAtom::parse(Lexer &lexer) {
  start_parsing("bool-atom");
  BoolAtom *atom = new BoolAtom();

  if (lexer.cur_token.kind == Token::Kind::TRUE)
    atom->b = true;
  else if (lexer.cur_token.kind == Token::Kind::FALSE)
    atom->b = false;
  lexer.advance();

  end_parsing("bool-atom");
  return atom;
}

NameAtom *NameAtom::parse(Lexer &lexer) {
  start_parsing("name-atom");
  NameAtom *atom = new NameAtom();

  expect(lexer, Token::Kind::NAME);
  atom->name = std::get<std::string>(lexer.cur_token.value);
  lexer.advance();

  end_parsing("name-atom");
  return atom;
}

InnerExpressionAtom *InnerExpressionAtom::parse(Lexer &lexer) {
  start_parsing("inner-expression-atom");
  InnerExpressionAtom *atom = new InnerExpressionAtom();

  skip(lexer, Token::Kind::LPAREN);
  atom->expression = Expression::parse(lexer);
  skip(lexer, Token::Kind::RPAREN);

  end_parsing("inner-expression-atom");
  return atom;
}

ListAtom *ListAtom::parse(Lexer &lexer) {
  start_parsing("list-atom");
  ListAtom *atom = new ListAtom();

  skip(lexer, Token::Kind::LBRACKET);
  while (true) {
    atom->list.push_back(Expression::parse(lexer));

    if (lexer.cur_token.kind == Token::Kind::RBRACKET)
      break;
    else
      skip(lexer, Token::Kind::COMMA);
  }
  skip(lexer, Token::Kind::RBRACKET);

  end_parsing("list-atom");
  return atom;
}

Atom *Atom::parse(Lexer &lexer) {
  start_parsing("atom");
  Atom *atom;

  switch (lexer.cur_token.kind) {
  case Token::Kind::CHAR_LITERAL:
    atom = CharAtom::parse(lexer);
    break;

  case Token::Kind::STRING_LITERAL:
    atom = StringAtom::parse(lexer);
    break;

  case Token::Kind::TRUE:
  case Token::Kind::FALSE:
    atom = BoolAtom::parse(lexer);
    break;

  case Token::Kind::NAME:
    atom = NameAtom::parse(lexer);
    break;

  case Token::Kind::LPAREN:
    atom = InnerExpressionAtom::parse(lexer);
    break;

  case Token::Kind::LBRACKET:
    atom = ListAtom::parse(lexer);
    break;

  default:
    // Unreachable
    assert(false);
  }

  end_parsing("atom");
  return atom;
}
