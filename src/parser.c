#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "nerror.h"
#include "nlist.h"
#include "parser.h"
#include "token.h"

// TODO: clean up forward declarations
static statement_t *parse_statement(lexer_t *lexer);

static int_constructor_t *parse_int_constructor(lexer_t *lexer) {
  int_constructor_t *intc = malloc(sizeof(*intc));
  lexer_advance(lexer);
  EXPECT_TOKEN(lexer, TOKEN_LPAREN);

  intc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(intc->statements, parse_statement(lexer));
  }

  EXPECT_TOKEN(lexer, TOKEN_RPAREN); // TODO: Unnecessary?
  lexer_advance(lexer);

  return intc;
}

static string_constructor_t *parse_string_constructor(lexer_t *lexer) {
  string_constructor_t *strc = malloc(sizeof(*strc));

  return strc;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
  fn_constructor_t *fnc = malloc(sizeof(*fnc));

  // parse params

  // call constructor of body type

  return fnc;
}

static constructor_t *parse_constructor(lexer_t *lexer) {
  constructor_t *constr = malloc(sizeof(*constr));
  lexer_advance(lexer);

  switch (lexer->cur_token->kind) {
  case TOKEN_INT:
    constr->int_constructor = parse_int_constructor(lexer);
    break;

  case TOKEN_STRING:
    constr->string_constructor = parse_string_constructor(lexer);
    break;

  case TOKEN_FN:
    constr->fn_constructor = parse_fn_constructor(lexer);
    break;

  default:
    err_illegal_token(lexer->cur_token);
  }

  return constr;
}
static primary_t *parse_primary(lexer_t *lexer) {
  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL) {
    lexer_free(lexer);
    err_malloc_fail();
  }

  return prm;
}

static unary_operator_t parse_factor_prefix(lexer_t *lexer) {
  // TODO: some sort of error handling
  if (lexer->cur_token->kind == TOKEN_PLUS) {
    return UNARY_PLUS;
  } else if (lexer->cur_token->kind == TOKEN_MINUS) {
    return UNARY_MINUS;
  } else if (lexer->cur_token->kind == TOKEN_NOT) {
    return UNARY_NOT;
  } else {
    return UNARY_NO_OP;
  }
}

static factor_t *parse_factor(lexer_t *lexer) {
  factor_t *ft = malloc(sizeof(*ft));
  if (ft == NULL) {
    lexer_free(lexer); // TODO: add macro/function for this repetetive cleanup?
    err_malloc_fail(); // TODO: add malloc fail checks everywhere.
  }

  // Check for prefixes
  ft->prefix = parse_factor_prefix(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

  return ft;
}

static binary_operator_t *parse_binary_operator(lexer_t *lexer) {
  // To avoid unnecessary malloc and free for return NULL path we make a
  // temporary varible to store the binary operators which will only be malloced
  // when needed
  binary_operator_t bo;

  switch (lexer->cur_token->kind) {
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
  default:
    return NULL;
  }

  binary_operator_t *ret = malloc(sizeof(*ret));
  *ret = bo;
  return ret;
}

// TODO: change condition to parse until not expr instead of newline/paren
static expr_t *parse_expr(lexer_t *lexer) {
  expr_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE, sizeof(factor_t));
  expr->operators = list_init(LIST_START_SIZE, sizeof(binary_operator_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    switch (lexer->cur_token->kind) {
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_NAME:
      list_push_back(expr->factors, parse_factor(lexer));
      list_push_back(expr->operators, parse_binary_operator(lexer));
      break;

    default:
      err_illegal_token(lexer->cur_token);
    }
  }

  return expr;
}

static definition_t *parse_definition(lexer_t *lexer) {
  EXPECT_TOKEN(lexer, TOKEN_DEF);
  lexer_advance(lexer);

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

  EXPECT_TOKEN(lexer, TOKEN_NAME);

  def->name = strdup(lexer->cur_token->name);

  lexer_advance(lexer);
  EXPECT_TOKEN(lexer, TOKEN_EQUALS);

  def->constructor = parse_constructor(lexer);

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
  assignment_t *ass = malloc(sizeof(*ass));

  return ass;
}

static statement_t *parse_statement(lexer_t *lexer) {
  statement_t *stmt = malloc(sizeof(*stmt));
  lexer_advance(lexer);

  switch (lexer->cur_token->kind) {
  case TOKEN_DEF:
    stmt->definition = parse_definition(lexer);
    break;

  case TOKEN_SET:
    stmt->assignment = parse_assignment(lexer);
    break;

  case TOKEN_NAME:
  case TOKEN_INTEGER_LITERAL:
  case TOKEN_STRING_LITERAL:
    stmt->expr = parse_expr(lexer);
    break;

  // TODO: Is this right?
  case TOKEN_EOF:
    free(stmt);
    return NULL;

  default:
    err_illegal_token(lexer->cur_token);
  }

  return stmt;
}

ast_t *parse(lexer_t *lexer) {
  ast_t *ast = malloc(sizeof(*ast));
  ast->statements = list_init(10, sizeof(statement_t));

  while (1) {
    statement_t *statement = parse_statement(lexer);
    printf("asd\n");
    list_push_back(ast->statements, statement);
    return ast;

    if (statement == NULL) {
      lexer_free(lexer);
      break;
    }

    list_push_back(ast->statements, statement);
  }

  return ast;
}

void ast_free(ast_t *ast) {
  for (size_t i = 0; i < ast->statements->cur_size; ++i) {
    // TODO: tricky
    // make generic parser_free_statement() or something
    // needs to not free the statement itself as it will be freed by list_free()
  }

  list_free(ast->statements);
  free(ast);
}
