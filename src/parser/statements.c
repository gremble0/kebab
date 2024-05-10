#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nerror.h"
#include "parser/constructors.h"
#include "parser/expressions.h"
#include "parser/logging.h"
#include "parser/statements.h"

// TODO: don't require constructor here, allow definition of primitives like
// `def a = 2` and infer type
static definition_t *definition_parse(lexer_t *lexer) {
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
  def->name = strdup(lexer->cur_token->name->s);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_EQUALS);

  def->constructor = constructor_parse(lexer);

  PARSER_LOG_NODE_FINISH("definition");

  return def;
}

static assignment_t *assignment_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("assignment");
  SKIP_TOKEN(lexer, TOKEN_SET);

  assignment_t *ass = malloc(sizeof(*ass));
  if (ass == NULL)
    err_malloc_fail();

  EXPECT_TOKEN(lexer, TOKEN_NAME);
  ass->name = strdup(lexer->cur_token->name->s);
  lexer_advance(lexer);

  SKIP_TOKEN(lexer, TOKEN_EQUALS);

  ass->constructor = constructor_parse(lexer);

  PARSER_LOG_NODE_FINISH("assignment");

  return ass;
}

statement_t *statement_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("statement");

  statement_t *stmt = malloc(sizeof(*stmt));
  if (stmt == NULL)
    err_malloc_fail();

  switch (lexer->cur_token->kind) {
  case TOKEN_DEF:
    stmt->type = STMT_DEFINITION;
    stmt->definition = definition_parse(lexer);
    break;

  case TOKEN_SET:
    stmt->type = STMT_ASSIGNMENT;
    stmt->assignment = assignment_parse(lexer);
    break;

  // I dont like this, its too exhaustive and needs to be updated all the
  // time. Should find a better way of starting expression parsing

  // Expressions can also be statements when used as
  // implicit returns Literals
  case TOKEN_NAME:
  case TOKEN_CHAR_LITERAL:
  case TOKEN_STRING_LITERAL:
  case TOKEN_INTEGER_LITERAL:
  // Cond expressions (if/elif/else)
  case TOKEN_IF:
  // Factor prefixes
  case TOKEN_PLUS:
  case TOKEN_MINUS:
  case TOKEN_MULT:
  case TOKEN_DIV:
  case TOKEN_NOT:
  // Booleans
  case TOKEN_TRUE:
  case TOKEN_FALSE:
  // Inner expression wrapped in parens, e.g. (1 + 2)
  case TOKEN_LPAREN:
  // Lists [x, y, z]
  case TOKEN_LBRACE:
    stmt->type = STMT_EXPRESSION;
    stmt->expr = expression_parse(lexer);
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
  constructor_free(ass->constructor);
  free(ass->name);
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
