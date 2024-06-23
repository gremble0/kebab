#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Token.hpp"

Lexer::Lexer(std::string path)
    : path(path), stream(path), line_number(1), line_pos(0), cur_token() {
  if (!stream.is_open())
    this->error("could not open file " + path);

  this->advance();
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
std::unique_ptr<Token> Lexer::read_number() {
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
    return std::make_unique<TokenFloatLiteral>(std::stof(&this->line[start_pos]));
  else
    return std::make_unique<TokenIntLiteral>(
        static_cast<int64_t>(std::stoi(&this->line[start_pos])));
}

std::unique_ptr<Token> Lexer::read_char() {
  // Verify there are enough chars in the line, an opening quote and a closing quote
  if (this->line_pos + 3 >= this->line.length() || this->peek(0) != '\'' || this->peek(2) != '\'')
    this->error("malformed char literal");

  auto c = std::make_unique<TokenFloatLiteral>(this->peek(1));
  this->line_pos += 3; // 1 for opening quote, 1 for char inside quotes, 1 for closing quote
  return c;
}

std::unique_ptr<Token> Lexer::read_string() {
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

  return std::make_unique<TokenStringLiteral>(this->line.substr(start_pos, end_pos - start_pos));
}

std::unique_ptr<Token> Lexer::read_word() {
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
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_DEF);
  else if (word.compare("set") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_SET);
  else if (word.compare("mut") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_MUT);
  else if (word.compare("if") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_IF);
  else if (word.compare("elif") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_ELIF);
  else if (word.compare("else") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_ELSE);
  else if (word.compare("fn") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_FN);
  else if (word.compare("true") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_TRUE);
  else if (word.compare("false") == 0)
    return std::make_unique<TokenSimple>(TokenKind::TOKEN_FALSE);
  else
    return std::make_unique<TokenName>(word);
}

void Lexer::advance() {
  char peeked = this->peek(0);

  switch (peeked) {
  // Nullbyte means end of the string effectively indicating a newline
  // Comment start means we ignore the rest of the line
  case '\0':
  case ';':
    this->next_line();

    if (stream.eof()) {
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_EOF);
      break;
    }

    this->advance();
    break;

  // Whitespace is simply ignored
  case '\t':
  case ' ':
    ++this->line_pos;
    this->advance();
    break;

  // Syntax
  case ':':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_COLON);
    break;
  case '=':
    // Token depends on the next character in the line
    switch (this->peek(1)) {
    case '>':
      this->line_pos += 2;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_FAT_RARROW);
      break;
    case '=':
      this->line_pos += 2;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_EQ);
      break;
    default:
      ++this->line_pos;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_EQUALS);
      break;
    }
    break;
  case ',':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_COMMA);
    break;
  case '(':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_LPAREN);
    break;
  case ')':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_RPAREN);
    break;
  case '[':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_LBRACKET);
    break;
  case ']':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_RBRACKET);
    break;

  // Operators
  case '+':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_PLUS);
    break;
  case '-':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_MINUS);
    break;
  case '*':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_MULT);
    break;
  case '~':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_NEQ);
      break;
    }

    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_NOT);
    break;
  case '<':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_LE);
      break;
    }

    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_LT);
    break;
  case '>':
    if (this->peek(1) == '=') {
      this->line_pos += 2;
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_GE);
      break;
    }

    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_GT);
    break;
  case '/':
    ++this->line_pos;
    this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_DIV);
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
      this->cur_token = std::make_unique<TokenSimple>(TokenKind::TOKEN_ILLEGAL);
      ++this->line_pos;
    }
    break;
  }
}
