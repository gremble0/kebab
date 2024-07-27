#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/logging.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nstring.h"
#include "utils/utils.h"

// TODO: Error if string or char doesnt have a closing quote.
//       Handle unexpected newlines better?

static int is_not_dquote(int c) { return c != '"'; }
static int is_kebab_case(int c) {
  return !isspace(c) && c != ',' && c != '(' && c != ')' && c != '[' && c != ']';
}

/**
 * @brief Dynamically loads the next line into the lexers buffer
 *
 * @param lexer lexer to load next chunk for
 */
static void lexer_load_next_line(lexer_t *lexer) {
  // Clear previous line (line is initialized to "" in lexer_init, so no special case for this)
  string_clear(lexer->line);

  size_t _ = 0;
  char *line_buf = NULL;
  ssize_t line_len = getline(&line_buf, &_, lexer->file.f);
  if (line_len >= 0) {
    string_set(lexer->line, line_buf, line_len);
    lexer->prev_line_pos = lexer->line_pos;
    lexer->line_pos = 0;
    ++lexer->line_number;
  }

  free(line_buf);
}

/**
 * @brief Seek forward in the current line in the lexer until a predicate fails.
 *
 * @param lexer lexer to check
 * @param pred a predicate function that takes a character, e.g. isalnum,
 * isdigit, isspace etc.
 * @return how many characters away from the current position the
 * predicate first fails.
 */
static size_t lexer_seek_while(const lexer_t *lexer, int pred(int)) {
  size_t i = 0;
  while (lexer->line_pos + i < lexer->line->len && pred(lexer->line->s[lexer->line_pos + i]))
    ++i;

  return i;
}

/**
 * @brief Peek forward in the current line by some offset and return the char
 * present there.
 *
 * @param lexer lexer to peek into
 * @param offset how many indexes to the right to peek
 * @return the character at the offset, 0 if offset is out of bounds
 */
static char lexer_peek_char(const lexer_t *lexer, size_t offset) {
  if (lexer->line_pos + offset >= lexer->line->len)
    return 0;

  return lexer->line->s[lexer->line_pos + offset];
}

/**
 * @brief Read digits at the current line until the isdigit predicate fails.
 * Also increments the lexer's line position
 *
 * @param lexer lexer to read the int from
 * @return the integer at the current line position
 */
static int64_t lexer_read_int(lexer_t *lexer) {
  ASSERT(isdigit(lexer->line->s[lexer->line_pos]));

  char *endptr;
  int64_t ret = strtoimax(&lexer->line->s[lexer->line_pos], &endptr, 10);
  size_t chars_read = endptr - &lexer->line->s[lexer->line_pos];
  lexer->line_pos += chars_read;

  return ret;
}

/**
 * @brief Reads characters between a pair of '"'-s. Also increments the lexer's
 * line position
 *
 * @param lexer lexer to read string from
 * @return the string between two '"'-s
 */
static string_t *lexer_read_str(lexer_t *lexer) {
  ASSERT(lexer->line->s[lexer->line_pos] == '"');
  ++lexer->line_pos;
  size_t i = lexer_seek_while(lexer, is_not_dquote);

  // string_dup adds nullbyte for us
  string_t *str = string_dup(&(string_t){lexer->line->s + lexer->line_pos, i});
  lexer->line_pos += i;
  ASSERT(lexer->line->s[lexer->line_pos] == '"');
  ++lexer->line_pos;

  return str;
}

/**
 * @brief Read a single character between a pair of '\''-s. Also increments the
 * lexer's line position
 *
 * @param lexer lexer to read char from
 * @return the char between the two '\''-s
 */
static uint8_t lexer_read_char(lexer_t *lexer) {
  ASSERT(lexer->line->s[lexer->line_pos] == '\'');
  ASSERT(lexer->line->s[lexer->line_pos + 2] == '\'');

  uint8_t c = lexer->line->s[lexer->line_pos + 1];

  lexer->line_pos += 3;

  return c;
}

/**
 * @brief Reads characters until whitespace or newline. Also increments the
 * lexer's line position
 *
 * @param lexer lexer to read word from
 * @return the string until the next whitespace or newline
 */
static string_t *lexer_read_word(lexer_t *lexer) {
  size_t i = lexer_seek_while(lexer, is_kebab_case);

  // string_dup adds nullbyte for us
  string_t *word = string_dup(&(string_t){lexer->line->s + lexer->line_pos, i});
  lexer->line_pos += i;

  return word;
}

/**
 * @param lexer to get span from
 * @return a span between the lexers previous and current position
 */
static span_t lexer_make_span(lexer_t *lexer) {
  return span_of(lexer->line_number, lexer->prev_line_pos, lexer->line_number, lexer->line_pos,
                 lexer->file);
}

/**
 * @brief Initialize a lexer, setting its inital state based on the file at
 * file_path. Returned lexer needs to be freed using lexer_free()
 *
 * @param file_path path to file to open lexer for
 * @return lexer at the start of the file at file_path
 */
lexer_t *lexer_init(const char *path) {
  LEXER_LOG_START();
  lexer_t *lexer = malloc(sizeof(*lexer));
  if (lexer == NULL)
    err_malloc_fail();

  FILE *f = fopen(path, "r");
  if (f == NULL)
    err_io_fail(path);

  lexer->file = (file_t){f, path};
  lexer->line = string_dup(&(string_t){NULL, 0});
  lexer->prev_line_pos = 0;
  lexer->line_pos = 0;
  lexer->line_number = 0;
  lexer->cur_token = NULL;

  // Load the first line
  lexer_load_next_line(lexer);
  // Load the first token
  lexer_advance(lexer);

  return lexer;
}

/**
 * @brief Advance lexer by loading the next token into lexer->cur_token. This
 * function also takes care of freeing all lexer->cur_tokens except for
 * TOKEN_EOF, which is freed by lexer_free.
 *
 * @param lexer lexer to advance
 */
void lexer_advance(lexer_t *lexer) {
  // Handle previous tokens and recursive calls
  if (lexer->cur_token != NULL) {
    // Handle previous token
    LEXER_LOG_TOKEN(lexer->cur_token);
    token_free(lexer->cur_token);
    // For recursive calls we reset cur_token to NULL to not double free
    lexer->cur_token = NULL;
  }

  lexer->prev_line_pos = lexer->line_pos; // For error handling

  switch (lexer->line->s[lexer->line_pos]) {
  // Newline or semicolon means go to next line
  // - Semicolon is comment start
  // - Kebab does not care about newlines so these are also ignored
  case '\n':
  case ';':
    lexer_load_next_line(lexer);
    // If line->s is NULL we have failed reading the next line indicating the end of the file
    if (lexer->line->s == NULL) {
      LEXER_LOG_FINISH();
      string_clear(lexer->line);
      lexer->cur_token = token_make_simple(TOKEN_EOF, (span_t){0});
      return;
    }
    return lexer_advance(lexer);

  // Whitespace
  case '\t':
  case ' ':
    ++lexer->line_pos;
    return lexer_advance(lexer);

  // Syntax
  case ':':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_COLON, lexer_make_span(lexer));
    return;

  case '=': {
    switch (lexer_peek_char(lexer, 1)) {
    case '>':
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_FAT_RARROW, lexer_make_span(lexer));
      return;
    case '=':
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_EQ, lexer_make_span(lexer));
      return;
    default:
      ++lexer->line_pos;
      lexer->cur_token = token_make_simple(TOKEN_EQUALS, lexer_make_span(lexer));
      return;
    }
  }

  case ',':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_COMMA, lexer_make_span(lexer));
    return;

  case '(':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LPAREN, lexer_make_span(lexer));
    return;

  case ')':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_RPAREN, lexer_make_span(lexer));
    return;

  case '[':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LBRACKET, lexer_make_span(lexer));
    return;

  case ']':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_RBRACKET, lexer_make_span(lexer));
    return;

  // Operators
  case '+':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_PLUS, lexer_make_span(lexer));
    return;

  case '-':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_MINUS, lexer_make_span(lexer));
    return;

  case '*':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_MULT, lexer_make_span(lexer));
    return;

  case '~':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_NEQ, lexer_make_span(lexer));
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_NOT, lexer_make_span(lexer));
    return;

  case '<':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_LE, lexer_make_span(lexer));
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LT, lexer_make_span(lexer));
    return;

  case '>':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_GE, lexer_make_span(lexer));
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_GT, lexer_make_span(lexer));
    return;

  case '/':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_DIV, lexer_make_span(lexer));
    return;

  // Literals
  case '"':
    lexer->cur_token = token_make_str_lit(lexer_read_str(lexer), lexer_make_span(lexer));
    return;

  case '\'':
    lexer->cur_token = token_make_char_lit(lexer_read_char(lexer), lexer_make_span(lexer));
    return;

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    lexer->cur_token = token_make_int_lit(lexer_read_int(lexer), lexer_make_span(lexer));
    return;

  // Keywords and builtin types
  default: {
    string_t *word = lexer_read_word(lexer);
    span_t span = lexer_make_span(lexer);

    // Check if the word is some reserved word
    if (string_compare(word, &token_string_def) == 0)
      lexer->cur_token = token_make_simple(TOKEN_DEF, span);
    else if (string_compare(word, &token_string_def) == 0)
      lexer->cur_token = token_make_simple(TOKEN_DEF, span);
    else if (string_compare(word, &token_string_set) == 0)
      lexer->cur_token = token_make_simple(TOKEN_SET, span);
    else if (string_compare(word, &token_string_mut) == 0)
      lexer->cur_token = token_make_simple(TOKEN_MUT, span);
    else if (string_compare(word, &token_string_if) == 0)
      lexer->cur_token = token_make_simple(TOKEN_IF, span);
    else if (string_compare(word, &token_string_elif) == 0)
      lexer->cur_token = token_make_simple(TOKEN_ELIF, span);
    else if (string_compare(word, &token_string_else) == 0)
      lexer->cur_token = token_make_simple(TOKEN_ELSE, span);
    else if (string_compare(word, &token_string_nil) == 0)
      lexer->cur_token = token_make_simple(TOKEN_NIL, span);

    else if (string_compare(word, &token_string_char) == 0)
      lexer->cur_token = token_make_simple(TOKEN_CHAR, span);
    else if (string_compare(word, &token_string_string) == 0)
      lexer->cur_token = token_make_simple(TOKEN_STRING, span);
    else if (string_compare(word, &token_string_int) == 0)
      lexer->cur_token = token_make_simple(TOKEN_INT, span);
    else if (string_compare(word, &token_string_bool) == 0)
      lexer->cur_token = token_make_simple(TOKEN_BOOL, span);
    else if (string_compare(word, &token_string_fn) == 0)
      lexer->cur_token = token_make_simple(TOKEN_FN, span);
    else if (string_compare(word, &token_string_list) == 0)
      lexer->cur_token = token_make_simple(TOKEN_LIST, span);

    else if (string_compare(word, &token_string_true) == 0)
      lexer->cur_token = token_make_simple(TOKEN_TRUE, span);
    else if (string_compare(word, &token_string_false) == 0)
      lexer->cur_token = token_make_simple(TOKEN_FALSE, span);

    else
      // If its not some reserved word its the name of some binding
      lexer->cur_token = token_make_name(word, span);

    return;
  }
  }
}

/**
 * @brief Free lexer and all data associated with it
 *
 * @param lexer lexer to free
 */
void lexer_free(lexer_t *lexer) {
  fclose(lexer->file.f);

  // If we try to free the lexer before we have reached the end of the file,
  // something has gone wrong
  ASSERT(lexer->cur_token->kind == TOKEN_EOF);
  token_free(lexer->cur_token);
  free(lexer);
}
