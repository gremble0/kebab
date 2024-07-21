#include <stdlib.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/expressions.h"
#include "parser/factors.h"
#include "parser/logging.h"
#include "parser/statements.h"
#include "utils/utils.h"

static binary_operator_t binary_operator_parse(lexer_t *lexer) {
  switch (lexer->cur_token->kind) {
  // Maths
  case TOKEN_PLUS:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='+'");
    lexer_advance(lexer);
    return BINARY_PLUS;
  case TOKEN_MINUS:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='-'");
    lexer_advance(lexer);
    return BINARY_MINUS;
  case TOKEN_MULT:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='*'");
    lexer_advance(lexer);
    return BINARY_MULT;
  case TOKEN_DIV:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='/'");
    lexer_advance(lexer);
    return BINARY_DIV;

  // Comparisons
  case TOKEN_LT:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='<'");
    lexer_advance(lexer);
    return BINARY_LT;
  case TOKEN_LE:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='<='");
    lexer_advance(lexer);
    return BINARY_LE;
  case TOKEN_EQ:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='=='");
    lexer_advance(lexer);
    return BINARY_EQ;
  case TOKEN_NEQ:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='~='");
    lexer_advance(lexer);
    return BINARY_NEQ;
  case TOKEN_GT:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='>'");
    lexer_advance(lexer);
    return BINARY_GT;
  case TOKEN_GE:
    PARSER_LOG_NODE_SELF_CLOSING("binary-operator='>='");
    lexer_advance(lexer);
    return BINARY_GE;

  default:
    return BINARY_NO_OP;
  }
}

static expression_t *cond_test_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-test");

  expression_t *test = expression_parse(lexer);

  PARSER_LOG_NODE_FINISH("cond-test");

  return test;
}

static list_t *cond_body_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-body");

  list_t *body = list_init(LIST_START_SIZE);

  while (1) {
    statement_t *stmt = statement_parse(lexer);
    list_push_back(body, stmt);

    if (stmt->type == STMT_EXPRESSION)
      break;
  }

  PARSER_LOG_NODE_FINISH("cond-body");

  return body;
}

static cond_t *cond_if_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-if");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_IF);
  cond->test = cond_test_parse(lexer);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = cond_body_parse(lexer);

  PARSER_LOG_NODE_FINISH("cond-if");

  return cond;
}

static cond_t *cond_elif_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-elif");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_ELIF);
  cond->test = cond_test_parse(lexer);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = cond_body_parse(lexer);

  PARSER_LOG_NODE_FINISH("cond-elif");

  return cond;
}

static cond_t *cond_else_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-else");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_ELSE);
  cond->test = NULL;
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = cond_body_parse(lexer);

  PARSER_LOG_NODE_FINISH("cond-else");

  return cond;
}

static expression_cond_t *expression_cond_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression-cond");

  expression_cond_t *excd = malloc(sizeof(*excd));
  if (excd == NULL)
    err_malloc_fail();

  excd->conds = list_init(LIST_START_SIZE);

  // parse 1 if
  list_push_back(excd->conds, cond_if_parse(lexer));

  // parse 0 or more elifs
  while (lexer->cur_token->kind == TOKEN_ELIF)
    list_push_back(excd->conds, cond_elif_parse(lexer));

  // parse 1 else (else is mandatory since expression must return something)
  list_push_back(excd->conds, cond_else_parse(lexer));

  PARSER_LOG_NODE_FINISH("expression-cond");

  return excd;
}

static expression_normal_t *expression_normal_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression-normal");

  expression_normal_t *exnr = malloc(sizeof(*exnr));
  if (exnr == NULL)
    err_malloc_fail();

  exnr->factors = list_init(LIST_START_SIZE); // list<factor_t *>
  // TODO: don't always init operators
  exnr->operators = list_init(LIST_START_SIZE); // list<binary_operator_t *>

  // Continue parsing a factor followed by a binary operator until there are no
  // more binary operators
  while (1) {
    list_push_back(exnr->factors, factor_parse(lexer));

    binary_operator_t bo = binary_operator_parse(lexer);
    if (bo == BINARY_NO_OP)
      break;

    binary_operator_t *bo_p = malloc(sizeof(*bo_p));
    *bo_p = bo;
    list_push_back(exnr->operators, bo_p);
  }

  PARSER_LOG_NODE_FINISH("expression-normal");

  return exnr;
}

static expression_constructor_t *expression_constructor_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression-constructor");

  expression_constructor_t *exco = malloc(sizeof(*exco));
  if (exco == NULL)
    err_malloc_fail();

  exco->constr = constructor_parse(lexer);

  PARSER_LOG_NODE_FINISH("expression-constructor");

  return exco;
}

expression_t *expression_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL)
    err_malloc_fail();

  expr->span.file = lexer->file;
  expr->span.start = (position_t){lexer->line_number, lexer->line_pos};

  switch (lexer->cur_token->kind) {
  case TOKEN_IF:
    expr->type = EXPR_COND;
    expr->cond = expression_cond_parse(lexer);
    break;

  case TOKEN_NAME:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_INT_LITERAL:
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_NOT:
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  case TOKEN_LPAREN:
  case TOKEN_LBRACKET:
    expr->type = EXPR_NORMAL;
    expr->normal = expression_normal_parse(lexer);
    break;

  case TOKEN_CHAR:
  case TOKEN_STRING:
  case TOKEN_INT:
  case TOKEN_LIST:
  case TOKEN_FN:
    expr->type = EXPR_CONSTRUCTOR;
    expr->constr = expression_constructor_parse(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  expr->span.end = (position_t){lexer->line_number, lexer->line_pos};

  PARSER_LOG_NODE_FINISH("expression");

  return expr;
}

static void cond_free(cond_t *cond) {
  if (cond->test != NULL)
    expression_free(cond->test);

  list_map(cond->body, (list_map_func)statement_free);
  list_free(cond->body);
  free(cond);
}

static void expression_cond_free(expression_cond_t *excd) {
  list_map(excd->conds, (list_map_func)cond_free);
  list_free(excd->conds);
  free(excd);
}

static void expression_normal_free(expression_normal_t *exnr) {
  list_map(exnr->factors, (list_map_func)factor_free);
  list_map(exnr->operators, free);

  list_free(exnr->factors);
  list_free(exnr->operators);
  free(exnr);
}

static void expression_constructor_free(expression_constructor_t *exco) {
  constructor_free(exco->constr);
  free(exco);
}

void expression_free(expression_t *expr) {
  switch (expr->type) {
  case EXPR_COND:
    expression_cond_free(expr->cond);
    break;
  case EXPR_NORMAL:
    expression_normal_free(expr->normal);
    break;
  case EXPR_CONSTRUCTOR:
    expression_constructor_free(expr->constr);
    break;
  }

  free(expr);
}
