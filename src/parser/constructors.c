#include <stdlib.h>

#include "constructors.h"
#include "nonstdlib/nlist.h"
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
  pc->statements = list_init(LIST_START_SIZE); // list<statement_t *>

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(pc->statements, parse_statement(lexer));
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
  fnc->params = list_init(LIST_START_SIZE); // list<fn_param_t *>

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnc->params, parse_fn_param(lexer));
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  // parse function body
  fnc->body = parse_constructor(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("fn_constructor");

  return fnc;
}

static list_constructor_t *parse_list_constructor(lexer_t *lexer) {
  START_PARSING("list_constructor");

  SKIP_TOKEN(TOKEN_LIST, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  list_constructor_t *lc = malloc(sizeof(*lc));
  lc->stmts = list_init(LIST_START_SIZE); // list<statement_t *>
  lc->type = parse_type(lexer);

  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(lc->stmts, parse_statement(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(TOKEN_COMMA, lexer);
    }
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("list_constructor");

  return lc;
}

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

static void fn_param_free(fn_param_t *fnp) {
  free(fnp->name);
  type_free(fnp->type);
  free(fnp);
}

static void primitive_constructor_free(primitive_constructor_t *pc) {
  list_map(pc->statements, (list_map_func)statement_free);
  list_free(pc->statements);
  free(pc);
}

static void fn_constructor_free(fn_constructor_t *fnc) {
  list_map(fnc->params, (list_map_func)fn_param_free);
  list_free(fnc->params);
  constructor_free(fnc->body);
  free(fnc);
}

static void list_constructor_free(list_constructor_t *lc) {
  list_map(lc->stmts, (list_map_func)statement_free);
  list_free(lc->stmts);
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
