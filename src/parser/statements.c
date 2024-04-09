#include <stdlib.h>
#include <string.h>

#include "constructors.h"
#include "factors.h"
#include "statements.h"
#include "utils.h"

static binary_operator_t parse_binary_operator(lexer_t *lexer) {
  binary_operator_t bo;

  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    lexer_advance(lexer);
    bo = BINARY_PLUS;
    break;
  case TOKEN_MINUS:
    lexer_advance(lexer);
    bo = BINARY_MINUS;
    break;
  case TOKEN_MULT:
    lexer_advance(lexer);
    bo = BINARY_MULT;
    break;
  case TOKEN_DIV:
    lexer_advance(lexer);
    bo = BINARY_DIV;
    break;
  default:
    bo = BINARY_NO_OP;
  }

#ifdef DEBUG
  if (bo != BINARY_NO_OP)
    start_and_finish_parsing("binary_operator");
#endif

  return bo;
}

// TODO: make static?
expression_t *parse_expression(lexer_t *lexer) {
  START_PARSING("expr");

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE, sizeof(factor_t));
  // TODO: don't always init operators
  expr->operators = list_init(LIST_START_SIZE, sizeof(binary_operator_t));

  // First parse one factor and binary operator
  factor_t *factor = parse_factor(lexer);
  list_push_back(expr->factors, factor);
  free(factor);
  binary_operator_t bo = parse_binary_operator(lexer);

  // Continue parsing until there are no more binary operators
  while (bo != BINARY_NO_OP) {
    switch (lexer->cur_token->kind) {
    case TOKEN_CHAR_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_NAME: {
      binary_operator_t *bo_p = malloc(sizeof(*bo_p));
      *bo_p = bo;
      factor_t *factor = parse_factor(lexer);

      // Add the operator from the last iteration, and the factor for this
      // iteration
      list_push_back(expr->operators, bo_p);
      list_push_back(expr->factors, factor);

      free(bo_p);
      free(factor);

      bo = parse_binary_operator(lexer); // operator for next iteration

      break;
    }

    default:
      err_illegal_token(lexer);
    }
  }

  FINISH_PARSING("expr");

  return expr;
}

// TODO: don't require constructor here, allow definition of primitives like
// `def a = 2` and infer type
static definition_t *parse_definition(lexer_t *lexer) {
  START_PARSING("definition");

  SKIP_TOKEN(TOKEN_DEF, lexer);

  definition_t *def = malloc(sizeof(*def));
  if (def == NULL) {
    err_malloc_fail();
  }

  // Definition can optionally be mutable, if omitted default is immutable
  if (lexer->cur_token->kind == TOKEN_MUT) {
    def->is_mutable = 1;
    lexer_advance(lexer);
  } else {
    def->is_mutable = 0;
  }

  EXPECT_TOKEN(TOKEN_NAME, lexer);
  def->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(TOKEN_EQUALS, lexer);

  def->constructor = parse_constructor(lexer);

  FINISH_PARSING("definition");

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
  START_PARSING("assignment");

  SKIP_TOKEN(TOKEN_SET, lexer);

  assignment_t *ass = malloc(sizeof(*ass));
  if (ass == NULL) {
    err_malloc_fail();
  }

  EXPECT_TOKEN(TOKEN_NAME, lexer);
  ass->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(TOKEN_EQUALS, lexer);

  ass->constructor = parse_constructor(lexer);

  FINISH_PARSING("assignment");

  return ass;
}

statement_t *parse_statement(lexer_t *lexer) {
  START_PARSING("statement");

  statement_t *stmt = malloc(sizeof(*stmt));
  if (stmt == NULL) {
    err_malloc_fail();
  }

  switch (lexer->cur_token->kind) {
  case TOKEN_DEF:
    stmt->type = STMT_DEFINITION;
    stmt->definition = parse_definition(lexer);
    break;

  case TOKEN_SET:
    stmt->type = STMT_ASSIGNMENT;
    stmt->assignment = parse_assignment(lexer);
    break;

  case TOKEN_NAME:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_INTEGER_LITERAL:
  // factor prefixes
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_NOT:
    stmt->type = STMT_EXPRESSION;
    stmt->expr = parse_expression(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  FINISH_PARSING("statement");

  return stmt;
}

static void definition_free(definition_t *def) {
  constructor_free(def->constructor);
  free((void *)def->name);
  free(def);
}

static void assignment_free(assignment_t *ass) {
  free((void *)ass->name);
  constructor_free(ass->constructor);
  free(ass);
}

/**
 * @param expr expression to free, should be type `expression_t`
 */
void expression_free(void *expr) {
  expression_t *e = expr;
  list_free(e->factors, factor_free);
  list_free(e->operators, free);
  free(expr);
}

/**
 * @brief Recursively free all data associated with a statement
 *
 * @param stmt statement to be freed. should be of type `statement_t`
 */
void statement_free(void *stmt) {
  statement_t *s = stmt;

  switch (s->type) {
  case STMT_DEFINITION:
    definition_free(s->definition);
    break;
  case STMT_ASSIGNMENT:
    assignment_free(s->assignment);
    break;
  case STMT_EXPRESSION:
    expression_free(s->expr);
    break;
  }

  free(s);
}
