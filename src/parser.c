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
#ifdef DEBUG
  printf("<int_constructor>\n");
#endif

  int_constructor_t *intc = malloc(sizeof(*intc));
  lexer_advance(lexer);
  EXPECT_TOKEN(lexer, TOKEN_LPAREN);

  intc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(intc->statements, parse_statement(lexer));
  }

  EXPECT_TOKEN(lexer, TOKEN_RPAREN); // TODO: Unnecessary?
  lexer_advance(lexer);

#ifdef DEBUG
  printf("</int_constructor>\n");
#endif

  return intc;
}

static string_constructor_t *parse_string_constructor(lexer_t *lexer) {
#ifdef DEBUG
  printf("<string_constructor>\n");
#endif

  string_constructor_t *strc = malloc(sizeof(*strc));

#ifdef DEBUG
  printf("</string_constructor>\n");
#endif

  return strc;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
#ifdef DEBUG
  printf("<fn_constructor>\n");
#endif

  fn_constructor_t *fnc = malloc(sizeof(*fnc));

  // parse params

  // call constructor of body type

#ifdef DEBUG
  printf("</fn_constructor>\n");
#endif

  return fnc;
}

// TODO: modularize parser into multiple files, e.g. parse_constructor,
// parse_atom, parse_def, etc.
static constructor_t *parse_constructor(lexer_t *lexer) {
#ifdef DEBUG
  printf("<constructor>\n");
#endif

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

#ifdef DEBUG
  printf("</constructor>\n");
#endif

  return constr;
}

static atom_t *parse_atom(lexer_t *lexer) {
#ifdef DEBUG
  printf("<atom>\n");
#endif

  atom_t *atom = malloc(sizeof(*atom));
  if (atom == NULL) {
    err_malloc_fail();
  }

  lexer_advance(lexer);
  atom->int_value = 5;

#ifdef DEBUG
  printf("</atom>\n");
#endif

  return atom;
}

static primary_t *parse_primary(lexer_t *lexer) {
#ifdef DEBUG
  printf("<primary>\n");
#endif

  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL) {
    lexer_free(lexer);
    err_malloc_fail();
  }

  prm->atom = parse_atom(lexer);

#ifdef DEBUG
  printf("</primary>\n");
#endif

  return prm;
}

// TODO: rename -> parse_unary_operator?
static unary_operator_t parse_factor_prefix(lexer_t *lexer) {
  unary_operator_t uo;

  // TODO: some sort of error handling
  switch (lexer->cur_token->kind) {
  case TOKEN_PLUS:
    lexer_advance(lexer);
    uo = UNARY_PLUS;
    break;
  case TOKEN_MINUS:
    lexer_advance(lexer);
    uo = UNARY_MINUS;
    break;
  case TOKEN_NOT:
    lexer_advance(lexer);
    uo = UNARY_NOT;
  default:
    uo = UNARY_NO_OP;
  }

#ifdef DEBUG
  if (uo != UNARY_NO_OP) {
    printf("<factor_prefix>\n");
    printf("</factor_prefix>\n");
  }
#endif

  return uo;
}

static factor_t *parse_factor(lexer_t *lexer) {
#ifdef DEBUG
  printf("<factor>\n");
#endif

  factor_t *ft = malloc(sizeof(*ft));
  if (ft == NULL) {
    lexer_free(lexer); // TODO: add macro/function for this repetetive cleanup?
    err_malloc_fail(); // TODO: add malloc fail checks everywhere.
  }

  // Check for prefixes
  ft->prefix = parse_factor_prefix(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

#ifdef DEBUG
  printf("</factor>\n");
#endif

  return ft;
}

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
  if (bo != BINARY_NO_OP) {
    printf("<binary_operator>\n");
    printf("</binary_operator>\n");
  }
#endif

  return bo;
}

// TODO: change condition to parse until not expr instead of newline/paren
static expr_t *parse_expr(lexer_t *lexer) {
#ifdef DEBUG
  printf("<expr>\n");
#endif

  expr_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE, sizeof(factor_t));
  expr->operators = list_init(LIST_START_SIZE, sizeof(binary_operator_t));

  // TODO: is condition always right?
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    switch (lexer->cur_token->kind) {
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_NAME:
      list_push_back(expr->factors, parse_factor(lexer));
      binary_operator_t bo = parse_binary_operator(lexer);
      if (bo != BINARY_NO_OP) {
        binary_operator_t *bo_p = malloc(sizeof(*bo_p));
        *bo_p = bo;
        list_push_back(expr->operators, bo_p);
      }
      break;

    default:
      err_illegal_token(lexer->cur_token);
    }
  }

#ifdef DEBUG
  printf("</expr>\n");
#endif

  return expr;
}

static definition_t *parse_definition(lexer_t *lexer) {
#ifdef DEBUG
  printf("<definition>\n");
#endif

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

#ifdef DEBUG
  printf("</definition>\n");
#endif

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
#ifdef DEBUG
  printf("<assignment>\n");
#endif

  assignment_t *ass = malloc(sizeof(*ass));

#ifdef DEBUG
  printf("</assignment>\n");
#endif

  return ass;
}

static statement_t *parse_statement(lexer_t *lexer) {
#ifdef DEBUG
  printf("<statement>\n");
#endif

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

#ifdef DEBUG
  printf("</statement>\n");
#endif

  return stmt;
}

ast_t *parse(lexer_t *lexer) {
#ifdef DEBUG
  printf("<ast>\n");
#endif

  ast_t *ast = malloc(sizeof(*ast));
  ast->statements = list_init(10, sizeof(statement_t));

  while (1) {
    statement_t *statement = parse_statement(lexer);
    list_push_back(ast->statements, statement);
    return ast;

    if (statement == NULL) {
      lexer_free(lexer);
      break;
    }

    list_push_back(ast->statements, statement);
  }

#ifdef DEBUG
  printf("</ast>\n");
#endif

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
