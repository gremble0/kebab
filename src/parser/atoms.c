#include <stdlib.h>
#include <string.h>

#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/atoms.h"
#include "parser/error.h"
#include "parser/expressions.h"
#include "parser/logging.h"
#include "parser/parser.h"
#include "utils/utils.h"

static expression_t *inner_expression_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("inner-expr");
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  expression_t *expr = expression_parse(lexer);

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("inner-expr");

  return expr;
}

static list_t *list_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("list");
  SKIP_TOKEN(lexer, TOKEN_LBRACE);

  list_t *list = list_init(LIST_START_SIZE);

  while (1) {
    list_push_back(list, expression_parse(lexer));
    if (lexer->cur_token->kind == TOKEN_COMMA)
      SKIP_TOKEN(lexer, TOKEN_COMMA);
    if (lexer->cur_token->kind == TOKEN_RBRACE)
      break;
  }

  SKIP_TOKEN(lexer, TOKEN_RBRACE);
  PARSER_LOG_NODE_FINISH("list");

  return list;
}

atom_t *atom_parse(lexer_t *lexer) {
  atom_t *atom = malloc(sizeof(*atom));
  if (atom == NULL)
    err_malloc_fail();

  span_t span = {
      .start = {.line = lexer->line_number, .col = lexer->prev_line_pos},
      .file = lexer->source_file,
  };

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR_LITERAL:
    atom->type = ATOM_CHAR;
    atom->char_value = lexer->cur_token->char_literal;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING(
        "char-literal='%c' span=[%zu, %zu] - [%zu, %zu]", atom->char_value,
        span.start.line, span.start.col, lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_STRING_LITERAL:
    atom->type = ATOM_STRING;
    atom->string_value = string_copy(lexer->cur_token->string_literal);
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING(
        "string-literal=\"%.*s\" span=[%zu, %zu] - [%zu, %zu]",
        (int)atom->string_value->len, atom->string_value->s, span.start.line,
        span.start.col, lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_INTEGER_LITERAL:
    atom->type = ATOM_INT;
    atom->int_value = lexer->cur_token->integer_literal;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING(
        "integer-literal=%ld span=[%zu, %zu] - [%zu, %zu]", atom->int_value,
        span.start.line, span.start.col, lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_NAME:
    atom->type = ATOM_NAME;
    atom->name_value = string_copy(lexer->cur_token->name);
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING(
        "name=\"%.*s\" span=[%zu, %zu] - [%zu, %zu]",
        (int)atom->name_value->len, atom->name_value->s, span.start.line,
        span.start.col, lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_TRUE:
    atom->type = ATOM_BOOL;
    atom->bool_value = 1;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("true span=[%zu, %zu] - [%zu, %zu]",
                                 span.start.line, span.start.col,
                                 lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_FALSE:
    atom->type = ATOM_BOOL;
    atom->bool_value = 0;
    lexer_advance(lexer);
    PARSER_LOG_NODE_SELF_CLOSING("false span=[%zu, %zu] - [%zu, %zu]",
                                 span.start.line, span.start.col,
                                 lexer->line_number, lexer->line_pos);
    break;

  case TOKEN_LPAREN:
    atom->type = ATOM_INNER_EXPR;
    atom->inner_expr_value = inner_expression_parse(lexer);
    break;

  case TOKEN_LBRACE:
    atom->type = ATOM_LIST;
    atom->list_value = list_parse(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  span.end = (position_t){.line = lexer->line_number, .col = lexer->line_pos};
  atom->span = span;

  return atom;
}

void atom_free(atom_t *atom) {
  switch (atom->type) {
  case ATOM_STRING:
    string_free(atom->string_value);
    break;
  case ATOM_NAME:
    string_free(atom->name_value);
    break;
  case ATOM_INNER_EXPR:
    expression_free(atom->inner_expr_value);
    break;
  case ATOM_LIST:
    list_map(atom->list_value, (list_map_func)expression_free);
    list_free(atom->list_value);
    break;
  case ATOM_CHAR:
  case ATOM_INT:
  case ATOM_BOOL:
    break;
  }

  free(atom);
}
