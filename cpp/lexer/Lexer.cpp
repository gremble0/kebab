#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(std::string path)
    : path(path), stream(path), line_number(1), line_pos(0), cur_token() {
  if (!stream.is_open())
    this->error("could not open file " + path);

  std::getline(this->stream, this->line);
}

[[noreturn]] void Lexer::error(std::string message) const {
  std::string where = this->path + ':' + std::to_string(this->line_number) + ':' +
                      std::to_string(this->line_pos) + '\n';
  std::cerr << where + "lexer-error: " + message << std::endl;

  exit(1);
}

void Lexer::next_line() {
  std::getline(this->stream, this->line);
  ++this->line_number;
  this->line_pos = 0;
}

char Lexer::peek(int offset) const {
  // Check if out of bounds
  if (this->line_pos + offset >= this->line.length() || this->line_pos + offset < 0)
    return '\0';

  return this->line[this->line_pos + offset];
}

// Returned token is either an integer literal or a float literal
Token Lexer::read_number() {
  size_t start_pos = this->line_pos;
  bool has_seen_point = false;

  while (true) {
    char peeked = this->peek(0);

    if (std::isalnum(peeked)) {
      ++this->line_pos;
    } else if (peeked == '.') {
      ++this->line_pos;
      if (has_seen_point)
        this->error("malformed float literal");

      has_seen_point = true;
    } else {
      break;
    }
  }

  if (has_seen_point)
    return Token(std::stof(&this->line[start_pos]));
  else
    return Token(static_cast<int64_t>(std::stoi(&this->line[start_pos])));
}

Token Lexer::read_char() {
  // Verify there are enough chars in the line, an opening quote and a closing quote
  if (this->line_pos + 3 >= this->line.length() || this->peek(0) != '\'' || this->peek(2) != '\'')
    this->error("malformed char literal");

  Token c(this->peek(1));
  this->line_pos += 3; // 1 for opening quote, 1 for char inside quotes, 1 for closing quote
  return c;
}

Token Lexer::read_string() {
  // Verify there are enough chars in the line for at least an opening and closing quote and that
  // the current char is an opening quote
  if (this->line_pos + 2 >= this->line.length() || this->peek(0) != '"')
    this->error("malformed string literal");

  size_t start_pos = ++this->line_pos; // Skip opening quote (start_pos is after this quote)

  while (true) {
    char peeked = this->peek(0);

    if (peeked == '\0')
      this->error("multiline strings are not supported");
    else if (peeked == '\\')
      ++this->line_pos;
    else if (peeked == '"')
      break;

    ++this->line_pos;
  }

  size_t end_pos = this->line_pos++; // Skip closing quote (end_pos is before this quote)

  return Token(TokenKind::TOKEN_STRING_LITERAL, this->line.substr(start_pos, end_pos - start_pos));
}

Token Lexer::read_word() {
  auto is_kebab_case = [](char c) {
    return !std::isspace(c) && c != ',' && c != '(' && c != ')' && c != '[' && c != ']';
  };

  // The first letter in the word should be a letter a-z
  if (!std::isalpha(this->peek(0)))
    this->error("words should start with a letter");

  size_t start_pos = this->line_pos;
  while (is_kebab_case(this->peek(0)))
    ++this->line_pos;
  size_t end_pos = this->line_pos;

  std::string word = this->line.substr(start_pos, end_pos - start_pos);

  if (word.compare("def") == 0)
    return Token(TokenKind::TOKEN_DEF);
  else if (word.compare("set") == 0)
    return Token(TokenKind::TOKEN_SET);
  else if (word.compare("mut") == 0)
    return Token(TokenKind::TOKEN_MUT);
  else if (word.compare("if") == 0)
    return Token(TokenKind::TOKEN_IF);
  else if (word.compare("elif") == 0)
    return Token(TokenKind::TOKEN_ELIF);
  else if (word.compare("else") == 0)
    return Token(TokenKind::TOKEN_ELSE);
  else if (word.compare("fn") == 0)
    return Token(TokenKind::TOKEN_FN);
  else if (word.compare("true") == 0)
    return Token(TokenKind::TOKEN_TRUE);
  else if (word.compare("false") == 0)
    return Token(TokenKind::TOKEN_FALSE);
  else
    return Token(TokenKind::TOKEN_NAME, word);
}

void Lexer::handle_newline() {
  char peeked = this->peek(0);
  assert(peeked == '\0' || peeked == ';');

  this->next_line();

  if (stream.eof()) {
    this->cur_token = Token(TokenKind::TOKEN_EOF);
    return;
  }

  this->advance();
}

void Lexer::handle_whitespace() {
  ++this->line_pos;
  this->advance();
}

void Lexer::handle_colon() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_COLON);
}

void Lexer::handle_equals() {
  assert(this->peek(0) == '=');

  switch (this->peek(1)) {
  case '>':
    this->line_pos += 2;
    this->cur_token = Token(TokenKind::TOKEN_FAT_RARROW);
    break;
  case '=':
    this->line_pos += 2;
    this->cur_token = Token(TokenKind::TOKEN_EQ);
    break;
  default:
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_EQUALS);
    break;
  }
}

void Lexer::handle_comma() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_COMMA);
}

void Lexer::handle_lparen() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_LPAREN);
}

void Lexer::handle_rparen() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_RPAREN);
}

void Lexer::handle_lbracket() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_LBRACKET);
}

void Lexer::handle_rbracket() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_RBRACKET);
}

void Lexer::handle_plus() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_PLUS);
}

void Lexer::handle_minus() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_MINUS);
}

void Lexer::handle_mult() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_MULT);
}

void Lexer::handle_not() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(TokenKind::TOKEN_NEQ);
  } else {
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_NOT);
  }
}

void Lexer::handle_lt() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(TokenKind::TOKEN_LE);
  } else {
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_LT);
  }
}

void Lexer::handle_gt() {
  if (this->peek(1) == '=') {
    this->line_pos += 2;
    this->cur_token = Token(TokenKind::TOKEN_GE);
  } else {
    ++this->line_pos;
    this->cur_token = Token(TokenKind::TOKEN_GT);
  }
}

void Lexer::handle_div() {
  ++this->line_pos;
  this->cur_token = Token(TokenKind::TOKEN_DIV);
}

void Lexer::advance() {
  char peeked = this->peek(0);

  switch (peeked) {
  // Nullbyte means end of the string effectively indicating a newline
  // Comment start means we ignore the rest of the line
  case '\0':
  case ';':
    this->handle_newline();
    break;

  // Whitespace is simply ignored
  case '\t':
  case ' ':
    this->handle_whitespace();
    break;

  // Syntax
  case ':':
    this->handle_colon();
    break;
  case '=':
    // Token depends on the next character in the line
    this->handle_equals();
    break;
  case ',':
    this->handle_comma();
    break;
  case '(':
    this->handle_lparen();
    break;
  case ')':
    this->handle_rparen();
    break;
  case '[':
    this->handle_lbracket();
    break;
  case ']':
    this->handle_rbracket();
    break;

  // Operators
  case '+':
    this->handle_plus();
    break;
  case '-':
    this->handle_minus();
    break;
  case '*':
    this->handle_mult();
    break;
  case '~':
    this->handle_not(); // tilde?
    break;
  case '<':
    this->handle_lt();
    break;
  case '>':
    this->handle_gt();
    break;
  case '/':
    this->handle_div(); // slash?
    break;

  // Literals
  case '"':
    this->cur_token = Lexer::read_string();
    break;

  case '\'':
    this->cur_token = Lexer::read_char();
    break;

  default:
    if (std::isalpha(peeked)) {
      this->cur_token = Lexer::read_word();
    } else if (std::isalnum(peeked)) {
      this->cur_token = Lexer::read_number();
    } else {
      this->cur_token = Token(TokenKind::TOKEN_ILLEGAL);
      ++this->line_pos;
    }
    break;
  }
}
