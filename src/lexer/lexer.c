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
  return !isspace(c) && c != ',' && c != '(' && c != ')' && c != '[' &&
         c != ']';
}

/**
 * @brief Dynamically loads the next line into the lexers buffer
 *
 * @param lexer lexer to load next chunk for
 */
static void lexer_load_next_line(lexer_t *lexer) {
  // unnecesary if?
  if (lexer->line != NULL) {
    free(lexer->line);
    lexer->line = NULL;
  }

  size_t _ = 0;
  lexer->line_len = getline(&lexer->line, &_, lexer->source_file.f);
  lexer->line_pos = 0;
  lexer->prev_line_pos = 0;
  ++lexer->line_number;
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
  while (lexer->line_pos + i < (size_t)lexer->line_len &&
         pred(lexer->line[lexer->line_pos + i]))
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
  if (lexer->line_pos + offset >= (size_t)lexer->line_len)
    return 0;

  return lexer->line[lexer->line_pos + offset];
}

/**
 * @brief Read digits at the current line until the isdigit predicate fails.
 * Also increments the lexer's line position
 *
 * @param lexer lexer to read the int from
 * @return the integer at the current line position
 */
static int64_t lexer_read_int(lexer_t *lexer) {
  ASSERT(isdigit(lexer->line[lexer->line_pos]));

  char *endptr;
  int64_t ret = strtoimax(&lexer->line[lexer->line_pos], &endptr, 10);
  size_t chars_read = endptr - &lexer->line[lexer->line_pos];
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
  ASSERT(lexer->line[lexer->line_pos] == '"');
  ++lexer->line_pos;
  size_t i = lexer_seek_while(lexer, is_not_dquote);

  string_t *str = string_of(lexer->line + lexer->line_pos, i);
  string_append_c(str, '\0');
  lexer->line_pos += i;
  ASSERT(lexer->line[lexer->line_pos] == '"');
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
  ASSERT(lexer->line[lexer->line_pos] == '\'');
  ASSERT(lexer->line[lexer->line_pos + 2] == '\'');

  uint8_t c = lexer->line[lexer->line_pos + 1];

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

  string_t *word = string_of(lexer->line + lexer->line_pos, i);
  // Kinda unnecessary realloc to do `string_append_c` but meh
  string_append_c(word, '\0');
  lexer->line_pos += i;

  return word;
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

  lexer->source_file = (file_t){
      .f = f,
      .name = path,
  };

  lexer->line = NULL;
  lexer->line_len = 0;
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

  switch (lexer->line[lexer->line_pos]) {
  // Newline or semicolon means go to next line
  // - Semicolon is comment start
  // - Kebab does not care about newlines so these are also ignored
  case '\n':
  case ';':
    lexer_load_next_line(lexer);
    // If line_len is negative reading the next line has failed indicating EOF
    if (lexer->line_len < 0) {
      LEXER_LOG_FINISH();
      free(lexer->line);
      lexer->cur_token = token_make_simple(TOKEN_EOF);
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
    lexer->cur_token = token_make_simple(TOKEN_COLON);
    return;
  case '=': {
    switch (lexer_peek_char(lexer, 1)) {
    case '>':
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_FAT_RARROW);
      return;
    case '=':
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_EQ);
      return;
    default:
      ++lexer->line_pos;
      lexer->cur_token = token_make_simple(TOKEN_EQUALS);
      return;
    }
  }
  case ',':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_COMMA);
    return;
  case '(':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LPAREN);
    return;
  case ')':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_RPAREN);
    return;
  case '[':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LBRACE);
    return;
  case ']':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_RBRACE);
    return;

  // Operators
  case '+':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_PLUS);
    return;
  case '-':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_MINUS);
    return;
  case '*':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_MULT);
    return;
  case '~':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_NEQ);
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_NOT);
    return;
  case '<':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_LE);
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_LT);
    return;
  case '>':
    if (lexer_peek_char(lexer, 1) == '=') {
      lexer->line_pos += 2;
      lexer->cur_token = token_make_simple(TOKEN_GE);
      return;
    }
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_GT);
    return;
  case '/':
    ++lexer->line_pos;
    lexer->cur_token = token_make_simple(TOKEN_DIV);
    return;

  // Literals
  case '"': {
    string_t *str = lexer_read_str(lexer);
    lexer->cur_token = token_make_str_lit(str);
    return;
  }
  case '\'':
    lexer->cur_token = token_make_char_lit(lexer_read_char(lexer));
    return;
  // TODO: revisit what to do with numbers starting with 0
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
    lexer->cur_token = token_make_int_lit(lexer_read_int(lexer));
    return;

  // Keywords and builtin types
  default: {
    string_t *word = lexer_read_word(lexer);

    // Check if its a reserved word
    for (size_t i = 0; i < ARRAY_SIZE(reserved_word_map); ++i) {
      if (string_compare(reserved_word_map[i].word, word) == 0) {
        string_free(word);
        lexer->cur_token = token_make_simple(reserved_word_map[i].kind);
        return;
      }
    }

    // If its not its just some identifier/name
    lexer->cur_token = token_make_name(word);
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
  fclose(lexer->source_file.f);

  // If we try to free the lexer before we have reached the end of the file,
  // something has gone wrong
  ASSERT(lexer->cur_token->kind == TOKEN_EOF);
  token_free(lexer->cur_token);
  free(lexer);
}
