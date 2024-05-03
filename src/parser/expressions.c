#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/expressions.h"
#include "parser/factors.h"
#include "parser/statements.h"
#include "parser/utils.h"

static binary_operator_t *parse_binary_operator(lexer_t *lexer) {
  binary_operator_t bo;

  switch (lexer->cur_token->kind) {
  // Maths
  case TOKEN_PLUS:
    bo = BINARY_PLUS;
    break;
  case TOKEN_MINUS:
    bo = BINARY_MINUS;
    break;
  case TOKEN_MULT:
    bo = BINARY_MULT;
    break;
  case TOKEN_DIV:
    bo = BINARY_DIV;
    break;

  // Comparisons
  case TOKEN_LT:
    bo = BINARY_LT;
    break;
  case TOKEN_LE:
    bo = BINARY_LE;
    break;
  case TOKEN_EQ:
    bo = BINARY_EQ;
    break;
  case TOKEN_NEQ:
    bo = BINARY_NEQ;
    break;
  case TOKEN_GT:
    bo = BINARY_GT;
    break;
  case TOKEN_GE:
    bo = BINARY_GE;
    break;

  // If next token is not a binary operator return without further processing
  default:
    return NULL;
  }

  lexer_advance(lexer);

  PARSER_LOG_NODE_SELF_CLOSING("binary-operator");

  binary_operator_t *ret = malloc(sizeof(*ret));
  *ret = bo;

  return ret;
}

static list_t *parse_cond_body(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-body");

  list_t *body = list_init(LIST_START_SIZE);

  while (1) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(body, stmt);

    if (stmt->type == STMT_EXPRESSION)
      break;
  }

  PARSER_LOG_NODE_FINISH("cond-body");

  return body;
}

static cond_t *parse_cond_if(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-if");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_IF);
  cond->test = parse_expression(lexer);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = parse_cond_body(lexer);

  PARSER_LOG_NODE_FINISH("cond-if");

  return cond;
}

static cond_t *parse_cond_elif(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-elif");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_ELIF);
  cond->test = parse_expression(lexer);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = parse_cond_body(lexer);

  PARSER_LOG_NODE_FINISH("cond-elif");

  return cond;
}

static cond_t *parse_cond_else(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-else");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  SKIP_TOKEN(lexer, TOKEN_ELSE);
  cond->test = NULL;
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  cond->body = parse_cond_body(lexer);

  PARSER_LOG_NODE_FINISH("cond-else");

  return cond;
}

static expr_cond_t *parse_expr_cond(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expr-cond");

  expr_cond_t *excd = malloc(sizeof(*excd));
  if (excd == NULL)
    err_malloc_fail();

  excd->conds = list_init(LIST_START_SIZE);

  // parse 1 if
  list_push_back(excd->conds, parse_cond_if(lexer));

  // parse 0 or more elifs
  while (lexer->cur_token->kind == TOKEN_ELIF)
    list_push_back(excd->conds, parse_cond_elif(lexer));

  // parse 1 else
  list_push_back(excd->conds, parse_cond_else(lexer));

  PARSER_LOG_NODE_FINISH("expr-cond");

  return excd;
}

// TODO: expression->expr, statement->stmt?
static expr_normal_t *parse_expr_normal(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expr-normal");

  expr_normal_t *exnr = malloc(sizeof(*exnr));
  if (exnr == NULL)
    err_malloc_fail();

  exnr->factors = list_init(LIST_START_SIZE); // list<factor_t *>
  // TODO: don't always init operators
  exnr->operators = list_init(LIST_START_SIZE); // list<binary_operator_t *>

  // Continue parsing until there are no more binary operators
  while (1) {
    list_push_back(exnr->factors, parse_factor(lexer));

    binary_operator_t *bo = parse_binary_operator(lexer);
    if (bo == NULL)
      break;

    list_push_back(exnr->operators, bo);
  }

  PARSER_LOG_NODE_FINISH("expr-normal");

  return exnr;
}

expression_t *parse_expression(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expr");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL)
    err_malloc_fail();

  switch (lexer->cur_token->kind) {
  case TOKEN_IF:
    expr->type = EXPR_COND;
    expr->cond = parse_expr_cond(lexer);
    break;

  // TODO: this is kinda shit, dont want default here
  default:
    expr->type = EXPR_NORMAL;
    expr->normal = parse_expr_normal(lexer);
  }

  PARSER_LOG_NODE_FINISH("expr");

  return expr;
}

expression_t *parse_inner_expression(lexer_t *lexer) {
  PARSER_LOG_NODE_START("inner-expr");

  SKIP_TOKEN(lexer, TOKEN_LPAREN);
  expression_t *expr = parse_expression(lexer);
  SKIP_TOKEN(lexer, TOKEN_RPAREN);

  PARSER_LOG_NODE_FINISH("inner-expr");

  return expr;
}

static void cond_free(cond_t *cond) {
  if (cond->test != NULL)
    expression_free(cond->test);

  list_map(cond->body, (list_map_func)statement_free);
  list_free(cond->body);
  free(cond);
}

static void expr_cond_free(expr_cond_t *excd) {
  list_map(excd->conds, (list_map_func)cond_free);
  list_free(excd->conds);
  free(excd);
}

static void expr_normal_free(expr_normal_t *exnr) {
  list_map(exnr->factors, (list_map_func)factor_free);
  list_map(exnr->operators, free);

  list_free(exnr->factors);
  list_free(exnr->operators);
  free(exnr);
}

void expression_free(expression_t *expr) {
  switch (expr->type) {
  case EXPR_COND:
    expr_cond_free(expr->cond);
    break;
  case EXPR_NORMAL:
    expr_normal_free(expr->normal);
    break;
  }

  free(expr);
}
