#include <stdlib.h>

#include "lexer/lexer.h"
#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/expressions.h"
#include "parser/factors.h"
#include "parser/statements.h"
#include "parser/utils.h"

static binary_operator_t parse_binary_operator(lexer_t *lexer) {
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

static expression_t *parse_cond_test(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond-test");

  expression_t *test = parse_expression(lexer);

  PARSER_LOG_NODE_FINISH("cond-test");

  return test;
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
  cond->test = parse_cond_test(lexer);
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
  cond->test = parse_cond_test(lexer);
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

static expression_cond_t *parse_expression_cond(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression-cond");

  expression_cond_t *excd = malloc(sizeof(*excd));
  if (excd == NULL)
    err_malloc_fail();

  excd->conds = list_init(LIST_START_SIZE);

  // parse 1 if
  list_push_back(excd->conds, parse_cond_if(lexer));

  // parse 0 or more elifs
  while (lexer->cur_token->kind == TOKEN_ELIF)
    list_push_back(excd->conds, parse_cond_elif(lexer));

  // parse 1 else (else is mandatory since expression must return something)
  list_push_back(excd->conds, parse_cond_else(lexer));

  PARSER_LOG_NODE_FINISH("expression-cond");

  return excd;
}

static void expression_normal_fix_precedence(expression_normal_t *exnr) {
  // A variant of insertionsort that sorts the factors in an expression by the
  // precedence of the operators between them
  for (size_t i = 1; i < exnr->operators->cur_size; ++i) {
    for (size_t j = i;
         j > 0 &&
         precedences[*(binary_operator_t *)list_get(exnr->operators, j - 1)] >
             precedences[*(binary_operator_t *)list_get(exnr->operators, j)];
         --j) {
      // Bubble the two factors between the operator one step to the left
      factor_t *fac1 = exnr->factors->entries[j];
      factor_t *fac2 = exnr->factors->entries[j + 1];
      exnr->factors->entries[j] = exnr->factors->entries[j - 1];
      exnr->factors->entries[j - 1] = fac1;
      exnr->factors->entries[j + 1] = exnr->factors->entries[j];
      exnr->factors->entries[j] = fac2;

      // Bubble operator one step to the left
      binary_operator_t *bo = exnr->operators->entries[j];
      exnr->operators->entries[j] = exnr->operators->entries[j - 1];
      exnr->operators->entries[j - 1] = bo;
    }
  }
}

static expression_normal_t *parse_expression_normal(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression-normal");

  expression_normal_t *exnr = malloc(sizeof(*exnr));
  if (exnr == NULL)
    err_malloc_fail();

  exnr->factors = list_init(LIST_START_SIZE); // list<factor_t *>
  // TODO: don't always init operators
  exnr->operators = list_init(LIST_START_SIZE); // list<binary_operator_t *>

  // Continue parsing until there are no more binary operators.
  while (1) {
    list_push_back(exnr->factors, parse_factor(lexer));

    binary_operator_t bo = parse_binary_operator(lexer);
    if (bo == BINARY_NO_OP)
      break;

    binary_operator_t *bo_p = malloc(sizeof(*bo_p));
    *bo_p = bo;
    list_push_back(exnr->operators, bo_p);
  }

  expression_normal_fix_precedence(exnr);

  // Order the factors by binary operator precedence

  PARSER_LOG_NODE_FINISH("expression-normal");

  return exnr;
}

expression_t *parse_expression(lexer_t *lexer) {
  PARSER_LOG_NODE_START("expression");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL)
    err_malloc_fail();

  switch (lexer->cur_token->kind) {
  case TOKEN_IF:
    expr->type = EXPR_COND;
    expr->cond = parse_expression_cond(lexer);
    break;

  case TOKEN_NAME:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_INTEGER_LITERAL:
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_NOT:
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  case TOKEN_LPAREN:
  case TOKEN_LBRACE:
    expr->type = EXPR_NORMAL;
    expr->normal = parse_expression_normal(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

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

void expression_free(expression_t *expr) {
  switch (expr->type) {
  case EXPR_COND:
    expression_cond_free(expr->cond);
    break;
  case EXPR_NORMAL:
    expression_normal_free(expr->normal);
    break;
  }

  free(expr);
}
