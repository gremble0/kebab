#include <stdlib.h>
#include <string.h>

#include "constructors.h"
#include "nonstdlib/nlist.h"
#include "statements.h"
#include "utils.h"

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
  case STMT_EXPRESSION:
    expression_free(stmt->expr);
    break;
  }

  free(stmt);
}

void statement_build_strs(statement_t *stmt, list_t *strs) {
  switch (stmt->type) {
  case STMT_EXPRESSION:
    list_push_back(strs, "<stmt type='expression'>");
    break;
  case STMT_ASSIGNMENT:
    list_push_back(strs, "<stmt type='assignment'>");
    break;
  case STMT_DEFINITION:
    list_push_back(strs, "<stmt type='definition'>");
    break;
  }

  list_push_back(strs, "</stmt>");
}
