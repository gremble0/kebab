#include <stdlib.h>

#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/statements.h"
#include "parser/types.h"
#include "parser/utils.h"
#include "string.h"

static primitive_constructor_t *parse_primitive_constructor(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primitive-constructor");

  lexer_advance(lexer); // current token should be one of the primitive types
                        // such as int, char string etc
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  primitive_constructor_t *pc = malloc(sizeof(*pc));
  pc->statements = list_init(LIST_START_SIZE); // list<statement_t *>

  while (lexer->cur_token->kind != TOKEN_RPAREN)
    list_push_back(pc->statements, parse_statement(lexer));

  SKIP_TOKEN(lexer, TOKEN_RPAREN);

  PARSER_LOG_NODE_FINISH("primitive-constructor");

  return pc;
}

static fn_param_t *parse_fn_param(lexer_t *lexer) {
  PARSER_LOG_NODE_START("fn-param");

  fn_param_t *param = malloc(sizeof(*param));

  EXPECT_TOKEN(lexer, TOKEN_NAME);
  param->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_COLON);

  param->type = parse_type(lexer);

  // Next token should be comma unless its the final param (then it should be
  // rparen, which will terminate the loop)
  if (lexer->cur_token->kind != TOKEN_RPAREN)
    SKIP_TOKEN(lexer, TOKEN_COMMA);

  PARSER_LOG_NODE_FINISH("fn-param");

  return param;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
  PARSER_LOG_NODE_START("fn-constructor");

  SKIP_TOKEN(lexer, TOKEN_FN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  fn_constructor_t *fnc = malloc(sizeof(*fnc));
  fnc->params = list_init(LIST_START_SIZE); // list<fn_param_t *>

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN)
    list_push_back(fnc->params, parse_fn_param(lexer));

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);

  // parse function body
  fnc->body = parse_constructor(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(lexer, TOKEN_RPAREN);

  PARSER_LOG_NODE_FINISH("fn-constructor");

  return fnc;
}

static list_constructor_t *parse_list_constructor(lexer_t *lexer) {
  PARSER_LOG_NODE_START("list-constructor");

  SKIP_TOKEN(lexer, TOKEN_LIST);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  list_constructor_t *lc = malloc(sizeof(*lc));
  lc->stmts = list_init(LIST_START_SIZE);
  lc->values = list_init(LIST_START_SIZE);
  lc->type = parse_type(lexer);

  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);

  // Parse any eventual statements preceding the return value
  // For example:
  // list(int =>
  //   def a = int(1) ; This will be parsed by this loop
  //   def b = int(2) ; This will be parsed by this loop
  //   a, b ; this starts with a STMT_EXPRESSION so we break after parsing `a`
  // )
  statement_t *stmt = parse_statement(lexer);
  while (stmt->type != STMT_EXPRESSION) {
    list_push_back(lc->stmts, stmt);
    stmt = parse_statement(lexer);
  }

  // Parse return value of constructor
  // TODO: ideally lc->values is just a list of expressions, not statement
  // expressions
  list_push_back(lc->values, stmt);
  while (1) {
    if (lexer->cur_token->kind == TOKEN_RPAREN)
      break;
    else
      SKIP_TOKEN(lexer, TOKEN_COMMA);

    size_t lexer_pos_before = lexer->prev_pos; // For error handling
    stmt = parse_statement(lexer);
    list_push_back(lc->values, stmt);

    if (stmt->type != STMT_EXPRESSION)
      err_illegal_statement(lexer, lexer_pos_before);
  }

  SKIP_TOKEN(lexer, TOKEN_RPAREN);

  PARSER_LOG_NODE_FINISH("list-constructor");

  return lc;
}

constructor_t *parse_constructor(lexer_t *lexer) {
  PARSER_LOG_NODE_START("constructor");

  constructor_t *constr = malloc(sizeof(*constr));

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR:
    constr->type = CONSTR_CHAR;
    constr->primitive_constructor = parse_primitive_constructor(lexer);
    break;

  case TOKEN_STRING:
    constr->type = CONSTR_STRING;
    constr->primitive_constructor = parse_primitive_constructor(lexer);
    break;

  case TOKEN_INT:
    constr->type = CONSTR_INT;
    constr->primitive_constructor = parse_primitive_constructor(lexer);
    break;

  case TOKEN_BOOL:
    constr->type = CONSTR_BOOL;
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

  PARSER_LOG_NODE_FINISH("constructor");

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

  list_map(lc->values, (list_map_func)statement_free);
  list_free(lc->values);

  type_free(lc->type);
  free(lc);
}

void constructor_free(constructor_t *constr) {
  switch (constr->type) {
  case CONSTR_CHAR:
  case CONSTR_STRING:
  case CONSTR_INT:
  case CONSTR_BOOL:
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
