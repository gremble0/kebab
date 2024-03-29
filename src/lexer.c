#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nerror.h"
#include "lexer.h"
#include "token.h"

static int is_not_dquote(int c) { return c != '"'; }
static int is_kebab_case(int c) {
  return !isspace(c) && c != ',' && c != '(' && c != ')';
}

/**
 * @brief Loads the LEXER_BUF_SIZE next bytes into the lexers buffer for reading
 * the source file, also resets buffer_pos
 *
 * @param lexer lexer to load next chunk for
 */
static void lexer_load_next_line(lexer_t *lexer) {
  if (lexer->line != NULL) {
    free(lexer->line);
  }

  size_t _ = 0;
  lexer->line_len = getline(&lexer->line, &_, lexer->source_file);
  lexer->line_pos = 0;
}

/**
 * @brief Checks if the lexer is done lexing the current line
 *
 * @param lexer lexer to check
 * @return 1 if line is done 0 if not
 */
static int lexer_line_is_done(lexer_t *lexer) {
  return lexer->line[lexer->line_pos] == '\n' ||
         lexer->line[lexer->line_pos] == ';';
}

/**
 * @brief Check if the current line is empty (or only contains a comment)
 *
 * @param lexer lexer to check
 * @return 1 if line is empty 0 if not
 */
static int lexer_line_is_empty(lexer_t *lexer) {
  if (lexer->line_len == 1) {
    return 1;
  }

  size_t spaces = 0;
  while (isspace(lexer->line[spaces])) {
    ++spaces;
  }

  return lexer->line[spaces] == ';';
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
static size_t lexer_seek_while(lexer_t *lexer, int pred(int)) {
  size_t i = 0;
  // TODO: && line->pos < line->line_len
  while (pred(lexer->line[lexer->line_pos + i])) {
    ++i;
  }

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
static char lexer_peek_char(lexer_t *lexer, size_t offset) {
  if (lexer->line_pos + offset >= (size_t)lexer->line_len) {
    return 0;
  }

  return lexer->line[lexer->line_pos + offset];
}

/**
 * @brief Read digits at the current line until the isdigit predicate fails.
 * Also increments the lexer's line position
 *
 * @param lexer lexer to read the int from
 * @return the integer at the current line position
 */
static int lexer_read_int(lexer_t *lexer) {
  assert(isdigit(lexer->line[lexer->line_pos]));
  size_t i = lexer_seek_while(lexer, isdigit);

  char num[i + 1];
  memcpy(num, &lexer->line[lexer->line_pos], i);
  num[i] = '\0';
  lexer->line_pos += i;

  return atoi(num);
}

/**
 * @brief Reads characters between a pair of '"'-s. Also increments the lexer's
 * line position
 *
 * @param lexer lexer to read string from
 * @return the string between two '"'-s
 */
static const char *lexer_read_str(lexer_t *lexer) {
  assert(lexer->line[lexer->line_pos] == '"');
  ++lexer->line_pos;
  size_t i = lexer_seek_while(lexer, is_not_dquote);

  char *word = malloc(sizeof(char) * i + 1);
  if (word == NULL) {
    err_malloc_fail();
  }

  memcpy(word, &lexer->line[lexer->line_pos], i);
  word[i] = '\0';
  assert(lexer->line[lexer->line_pos + i] == '"');
  lexer->line_pos += i + 1; // +1 to skip closing '"'

  return word;
}

static char lexer_read_char(lexer_t *lexer) {
  assert(lexer->line[lexer->line_pos] == '\'');
  assert(lexer->line[lexer->line_pos + 2] == '\'');

  char c = lexer->line[lexer->line_pos + 1];

  lexer->line_pos += 3;

  return c;
}

// TODO: widen condition to allow true KEBAB-CASE !!
/**
 * @brief Reads characters until whitespace or newline. Also increments the
 * lexer's line position
 *
 * @param lexer lexer to read word from
 * @return the string until the next whitespace or newline
 */
static const char *lexer_read_word(lexer_t *lexer) {
  size_t i = lexer_seek_while(lexer, is_kebab_case);

  char *word = malloc(sizeof(char) * i + 1);
  if (word == NULL) {
    err_malloc_fail();
  }

  memcpy(word, &lexer->line[lexer->line_pos], i);
  word[i] = '\0';
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
lexer_t *lexer_init(const char *file_path) {
  lexer_t *lexer = malloc(sizeof(lexer_t));
  if (lexer == NULL) {
    err_malloc_fail();
  }

  FILE *f = fopen(file_path, "r");
  if (f == NULL) {
    err_io_fail(file_path);
  }
  lexer->source_file = f;
  lexer->line = NULL;
  lexer_load_next_line(lexer);

  return lexer;
}

/**
 * @brief Get the next token in the lexer, returned token needs to be freed
 *
 * @param lexer lexer to get token from
 * @return token with appropriate contents based on lexer position
 */
token_t *lexer_next_token(lexer_t *lexer) {
  if (lexer->line_len < 0) {
    free(lexer->line);
    return token_make_simple(TOKEN_EOF);
  }

  if (lexer_line_is_done(lexer)) {
    if (lexer_line_is_empty(lexer)) {
      lexer_load_next_line(lexer);
      return lexer_next_token(lexer);
    } else {
      lexer_load_next_line(lexer);
      return token_make_simple(TOKEN_EOL);
    }
  }

  switch (lexer->line[lexer->line_pos]) {
  // Whitespace
  case '\t':
  case ' ':
    ++lexer->line_pos;
    return lexer_next_token(lexer);

    // Syntax
  case ':':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_COLON);
  case '=': {
    if (lexer_peek_char(lexer, 1) == '>') {
      lexer->line_pos += 2;
      return token_make_simple(TOKEN_FAT_RARROW);
    } else {
      ++lexer->line_pos;
      return token_make_simple(TOKEN_EQUALS);
    }
  }
  case ',':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_COMMA);
  case '(':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_LPAREN);
  case ')':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_RPAREN);
  case '[':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_LBRACE);
  case ']':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_RBRACE);

  // Primitives
  case '+':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_PLUS);
  case '-':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_MINUS);
  case '*':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_MULT);
  case '/':
    ++lexer->line_pos;
    return token_make_simple(TOKEN_DIV);

  case '"':
    return token_make_str_lit(lexer_read_str(lexer));
  case '\'':
    return token_make_char_lit(lexer_read_char(lexer));

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
    return token_make_int_lit(lexer_read_int(lexer));

  default: {
    const char *word = lexer_read_word(lexer);
    // TODO: hash and switch case
    if (strcmp(word, "def") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_DEF);

    } else if (strcmp(word, "set") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_SET);

    } else if (strcmp(word, "mut") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_MUT);

    } else if (strcmp(word, "int") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_INT);

    } else if (strcmp(word, "string") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_STRING);

    } else if (strcmp(word, "fn") == 0) {
      free((void *)word);
      return token_make_simple(TOKEN_FN);

    } else {
      return token_make_name(word);
    }
  }
  }
}

token_t *lexer_peek_token(lexer_t *lexer /*, size_t steps*/) {
  // TODO: Maybe assert is unnecessary
  ASSERT(lexer->line_pos != (size_t)lexer->line_len);
  size_t prev_pos = lexer->line_pos;

  token_t *token = lexer_next_token(lexer);
  lexer->line_pos = prev_pos;

  return token;
}

void lexer_skip_token(lexer_t *lexer, token_kind_t token_kind) {
  token_t *token = lexer_next_token(lexer);
  if (token->kind != token_kind) {
    lexer_free(lexer);
    err_illegal_token(token);
  }
  token_free(token);
}

/**
 * @brief Free lexer and all data associated with it
 *
 * @param lexer lexer to free
 */
void lexer_free(lexer_t *lexer) {
  fclose(lexer->source_file);
  free(lexer);
}
