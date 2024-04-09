#include <stdlib.h>

#include "constructors.h"
#include "statements.h"
#include "string.h"
#include "types.h"
#include "utils.h"

static primitive_constructor_t *parse_primitive_constructor(lexer_t *lexer) {
  START_PARSING("bool_constructor");

  // NAME?
  SKIP_TOKEN(TOKEN_NAME, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  primitive_constructor_t *pc = malloc(sizeof(*pc));
  pc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    // list_push_back copies
    statement_t *stmt = parse_statement(lexer);
    list_push_back(pc->statements, stmt);
    free(stmt);
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("bool_constructor");

  return pc;
}

static fn_param_t *parse_fn_param(lexer_t *lexer) {
  START_PARSING("fn_param");

  fn_param_t *param = malloc(sizeof(*param));

  EXPECT_TOKEN(TOKEN_NAME, lexer);
  param->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(TOKEN_COLON, lexer);

  param->type = parse_type(lexer);

  // Next token should be comma unless its the final param (then it should be
  // rparen, which will terminate the loop)
  if (lexer->cur_token->kind != TOKEN_RPAREN) {
    SKIP_TOKEN(TOKEN_COMMA, lexer);
  }

  FINISH_PARSING("fn_param");

  return param;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
  START_PARSING("fn_constructor");

  SKIP_TOKEN(TOKEN_FN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  fn_constructor_t *fnc = malloc(sizeof(*fnc));
  fnc->params = list_init(LIST_START_SIZE, sizeof(fn_param_t));

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    fn_param_t *fnp = parse_fn_param(lexer);
    list_push_back(fnc->params, fnp);
    free(fnp);
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  // parse function body
  fnc->body = parse_constructor(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  // TODO: make variadic and print debug info in finish_parsing/start_parsing
  FINISH_PARSING("fn_constructor");

  return fnc;
}

static list_constructor_t *parse_list_constructor(lexer_t *lexer) {
  START_PARSING("list_constructor");

  SKIP_TOKEN(TOKEN_LIST, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  list_constructor_t *lc = malloc(sizeof(*lc));
  lc->stmts = list_init(LIST_START_SIZE, sizeof(statement_t));
  lc->type = parse_type(lexer);

  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(lc->stmts, stmt);
    free(stmt);

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(TOKEN_COMMA, lexer);
    }
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("list_constructor");

  return lc;
}

// TODO: modularize parser into multiple files, e.g. parse_constructor,
// parse_atom, parse_def, etc.
constructor_t *parse_constructor(lexer_t *lexer) {
  START_PARSING("constructor");

  constructor_t *constr = malloc(sizeof(*constr));

  switch (lexer->cur_token->kind) {
  case TOKEN_NAME:
    constr->type = CONSTR_PRIMITIVE;
    constr->primitive_constructor = parse_primitive_constructor(lexer);
    break;

  case TOKEN_FN:
    constr->type = CONSTR_FN;
    constr->fn_constructor = parse_fn_constructor(lexer);
    break;

  case TOKEN_LIST:
    constr->type = CONSTR_LIST;
    constr->list_constructor = parse_list_constructor(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  FINISH_PARSING("constructor");

  return constr;
}

static void fn_param_free(void *fnp) {
  fn_param_t *f = fnp;
  free((void *)f->name);
  type_free(f->type);
  free(fnp);
}

// TODO: these free constructor functions are the same essentialy. Same other
// places. Should generalize
static void primitive_constructor_free(primitive_constructor_t *pc) {
  list_free(pc->statements, statement_free);
  free(pc);
}

static void fn_constructor_free(fn_constructor_t *fnc) {
  list_free(fnc->params, fn_param_free);
  constructor_free(fnc->body);
  free(fnc);
}

static void list_constructor_free(list_constructor_t *lc) {
  list_free(lc->stmts, statement_free);
  type_free(lc->type);
  free(lc);
}

void constructor_free(constructor_t *constr) {
  switch (constr->type) {
  case CONSTR_PRIMITIVE:
    primitive_constructor_free(constr->primitive_constructor);
    break;

  case CONSTR_FN:
    fn_constructor_free(constr->fn_constructor);
    break;

  case CONSTR_LIST:
    list_constructor_free(constr->list_constructor);
    break;
  }

  free(constr);
}