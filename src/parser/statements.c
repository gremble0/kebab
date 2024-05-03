#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "parser/constructors.h"
#include "parser/expressions.h"
#include "parser/statements.h"
#include "parser/utils.h"

// TODO: don't require constructor here, allow definition of primitives like
// `def a = 2` and infer type
static definition_t *parse_definition(lexer_t *lexer) {
  PARSER_LOG_NODE_START("definition");

  SKIP_TOKEN(lexer, TOKEN_DEF);

  definition_t *def = malloc(sizeof(*def));
  if (def == NULL)
    err_malloc_fail();

  // Definition can optionally be mutable, if omitted default is immutable
  if (lexer->cur_token->kind == TOKEN_MUT) {
    def->is_mutable = 1;
    lexer_advance(lexer);
  } else {
    def->is_mutable = 0;
  }

  EXPECT_TOKEN(lexer, TOKEN_NAME);
  def->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_EQUALS);

  def->constructor = parse_constructor(lexer);

  PARSER_LOG_NODE_FINISH("definition");

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
  PARSER_LOG_NODE_START("assignment");

  SKIP_TOKEN(lexer, TOKEN_SET);

  assignment_t *ass = malloc(sizeof(*ass));
  if (ass == NULL)
    err_malloc_fail();

  EXPECT_TOKEN(lexer, TOKEN_NAME);
  ass->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_EQUALS);

  ass->constructor = parse_constructor(lexer);

  PARSER_LOG_NODE_FINISH("assignment");

  return ass;
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

static cond_t *parse_cond(lexer_t *lexer) {
  PARSER_LOG_NODE_START("cond");

  cond_t *cond = malloc(sizeof(*cond));
  if (cond == NULL)
    err_malloc_fail();

  cond->tests = list_init(LIST_START_SIZE);
  cond->bodies = list_init(LIST_START_SIZE);

  // parse 1 if
  SKIP_TOKEN(lexer, TOKEN_IF);
  list_push_back(cond->tests, parse_expression(lexer));
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  list_push_back(cond->bodies, parse_cond_body(lexer));

  // parse 0 or more elifs
  while (lexer->cur_token->kind == TOKEN_ELIF) {
    SKIP_TOKEN(lexer, TOKEN_ELIF);
    list_push_back(cond->tests, parse_expression(lexer));
    SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
    list_push_back(cond->bodies, parse_cond_body(lexer));
  }

  // parse 1 else
  SKIP_TOKEN(lexer, TOKEN_ELSE);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  list_push_back(cond->bodies, parse_cond_body(lexer));

  PARSER_LOG_NODE_FINISH("cond");

  return cond;
}

statement_t *parse_statement(lexer_t *lexer) {
  PARSER_LOG_NODE_START("statement");

  statement_t *stmt = malloc(sizeof(*stmt));
  if (stmt == NULL)
    err_malloc_fail();

  switch (lexer->cur_token->kind) {
  case TOKEN_DEF:
    stmt->type = STMT_DEFINITION;
    stmt->definition = parse_definition(lexer);
    break;

  case TOKEN_SET:
    stmt->type = STMT_ASSIGNMENT;
    stmt->assignment = parse_assignment(lexer);
    break;

  case TOKEN_IF:
    stmt->type = STMT_COND;
    stmt->cond = parse_cond(lexer);
    break;

  // Expressions can also be statements (expression statements)
  // Literals
  case TOKEN_NAME:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_INTEGER_LITERAL:
  // Factor prefixes
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_NOT:
  // Inner expression wrapped in parens, e.g. (1 + 2)
  case TOKEN_LPAREN:
    stmt->type = STMT_EXPRESSION;
    stmt->expr = parse_expression(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  PARSER_LOG_NODE_FINISH("statement");

  return stmt;
}

static void definition_free(definition_t *def) {
  constructor_free(def->constructor);
  free(def->name);
  free(def);
}

static void assignment_free(assignment_t *ass) {
  free(ass->name);
  constructor_free(ass->constructor);
  free(ass);
}

static void cond_free(cond_t *cond) {
  list_map(cond->tests, (list_map_func)expression_free);
  list_map(cond->bodies, (list_map_func)statement_free);
  free(cond);
}

/**
 * @brief Recursively free all data associated with a statement
 *
 * @param stmt statement to be freed. should be of type `statement_t`
 */
void statement_free(statement_t *stmt) {
  switch (stmt->type) {
  case STMT_DEFINITION:
    definition_free(stmt->definition);
    break;

  case STMT_ASSIGNMENT:
    assignment_free(stmt->assignment);
    break;

  case STMT_COND:
    cond_free(stmt->cond);
    break;

  case STMT_EXPRESSION:
    expression_free(stmt->expr);
    break;
  }

  free(stmt);
}
