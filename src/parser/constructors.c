#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/constructors.h"
#include "parser/logging.h"
#include "parser/statements.h"
#include "parser/types.h"
#include "utils/utils.h"

static primitive_constructor_t *primitive_constructor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("primitive-constructor");
  lexer_advance(lexer); // current token should be one of the primitive types
                        // such as int, char string etc
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  primitive_constructor_t *pc = malloc(sizeof(*pc));
  pc->body = list_init(LIST_START_SIZE); // list<statement_t *>

  while (lexer->cur_token->kind != TOKEN_RPAREN)
    list_push_back(pc->body, statement_parse(lexer));

  // Last statement in constructor should be an expression (the return type of
  // the constructor)
  statement_t *last_stmt = list_get(pc->body, pc->body->size - 1);
  if (pc->body->size == 0 || last_stmt->type != STMT_EXPRESSION)
    err_missing_return(lexer);

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("primitive-constructor");

  return pc;
}

static fn_param_t *fn_param_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("fn-param");

  fn_param_t *param = malloc(sizeof(*param));

  EXPECT_TOKEN(lexer, TOKEN_NAME);
  param->name = string_copy(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_COLON);

  param->type = type_parse(lexer);

  // Next token should be comma unless its the final param (then it should be
  // rparen, which will terminate the loop)
  if (lexer->cur_token->kind != TOKEN_RPAREN)
    SKIP_TOKEN(lexer, TOKEN_COMMA);

  PARSER_LOG_NODE_FINISH("fn-param");

  return param;
}

static fn_constructor_t *fn_constructor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("fn-constructor");
  SKIP_TOKEN(lexer, TOKEN_FN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  fn_constructor_t *fnc = malloc(sizeof(*fnc));
  fnc->params = list_init(LIST_START_SIZE); // list<fn_param_t *>

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN)
    list_push_back(fnc->params, fn_param_parse(lexer));

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);

  // parse function body
  fnc->constr = constructor_parse(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("fn-constructor");

  return fnc;
}

static list_constructor_t *list_constructor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("list-constructor");
  SKIP_TOKEN(lexer, TOKEN_LIST);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  list_constructor_t *lc = malloc(sizeof(*lc));
  lc->body = list_init(LIST_START_SIZE);
  lc->type = type_parse(lexer);

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);

  while (lexer->cur_token->kind != TOKEN_RPAREN)
    list_push_back(lc->body, statement_parse(lexer));

  // Last statement in constructor should be an expression (the return type of
  // the constructor)
  statement_t *last_stmt = list_get(lc->body, lc->body->size - 1);
  if (lc->body->size == 0 || last_stmt->type != STMT_EXPRESSION)
    err_missing_return(lexer);

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("list-constructor");

  return lc;
}

constructor_t *constructor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("constructor");

  constructor_t *constr = malloc(sizeof(*constr));
  if (constr == NULL)
    err_malloc_fail();

  constr->span.file = lexer->file;
  constr->span.start = (position_t){lexer->line_number, lexer->line_pos};

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR:
    constr->primitive_constructor = primitive_constructor_parse(lexer);
    constr->type = type_char;
    break;

  case TOKEN_STRING:
    constr->primitive_constructor = primitive_constructor_parse(lexer);
    constr->type = type_string;
    break;

  case TOKEN_INT:
    constr->primitive_constructor = primitive_constructor_parse(lexer);
    constr->type = type_int;
    break;

  case TOKEN_BOOL:
    constr->primitive_constructor = primitive_constructor_parse(lexer);
    constr->type = type_bool;
    break;

  case TOKEN_FN:
    constr->fn_constructor = fn_constructor_parse(lexer);
    constr->type = malloc(sizeof(*constr->type));
    if (constr->type == NULL)
      err_malloc_fail();

    constr->type->fn_type = (keb_type_fn_t){
        // Return type of a function is the same as the type of the function
        // constructors body
        .return_type = constr->fn_constructor->constr->type,
        .param_types = list_init(LIST_START_SIZE),
    };

    constr->type->kind = TYPE_FN;

    for (size_t i = 0; i < constr->fn_constructor->params->size; ++i) {
      fn_param_t *param = list_get(constr->fn_constructor->params, i);
      list_push_back(constr->type->fn_type.param_types, param->type);
    }
    break;

  case TOKEN_LIST:
    // TODO: this and fn is shit
    constr->list_constructor = list_constructor_parse(lexer);
    constr->type = malloc(sizeof(*constr->type));
    if (constr->type == NULL)
      err_malloc_fail();

    constr->type->kind = TYPE_LIST;
    constr->type->list_type = (keb_type_list_t){
        .type = constr->list_constructor->type,
    };
    break;

  default:
    err_illegal_token(lexer);
  }

  // TODO: this seems to be wrong
  constr->span.end = (position_t){lexer->line_number, lexer->line_pos};

  PARSER_LOG_NODE_FINISH("constructor");

  return constr;
}

static void fn_param_free(fn_param_t *fnp) {
  string_free(fnp->name);
  type_free(fnp->type);
  free(fnp);
}

static void primitive_constructor_free(primitive_constructor_t *pc) {
  list_map(pc->body, (list_map_func)statement_free);
  list_free(pc->body);
  free(pc);
}

static void fn_constructor_free(fn_constructor_t *fnc) {
  list_map(fnc->params, (list_map_func)fn_param_free);
  list_free(fnc->params);
  constructor_free(fnc->constr);
  free(fnc);
}

static void list_constructor_free(list_constructor_t *lc) {
  list_map(lc->body, (list_map_func)statement_free);
  list_free(lc->body);

  type_free(lc->type);
  free(lc);
}

void constructor_free(constructor_t *constr) {
  switch (constr->type->kind) {
  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_BOOL:
    primitive_constructor_free(constr->primitive_constructor);
    break;

  case TYPE_FN:
    fn_constructor_free(constr->fn_constructor);
    break;

  case TYPE_LIST:
    list_constructor_free(constr->list_constructor);
    break;
  }

  free(constr);
}
