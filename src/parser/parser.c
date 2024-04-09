#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include "debug.h"
#endif
#include "lexer.h"
#include "nerror.h"
#include "nlist.h"
#include "parser.h"
#include "parser_error.h"
#include "token.h"

// TODO: clean up forward declarations
// These are the main functions that will be called recursively
static statement_t *parse_statement(lexer_t *lexer);
static constructor_t *parse_constructor(lexer_t *lexer);
static keb_type_t *parse_type(lexer_t *lexer);
static expression_t *parse_expression(lexer_t *lexer);

static keb_type_fn_t *parse_type_fn(lexer_t *lexer) {
  // function types should look something like `fn(int, int) => int`
  // for example:
  //     def fn-showcase = fn(some-param : fn(int, int) => int) => ...
  START_PARSING("fn_type");

  SKIP_TOKEN(TOKEN_FN, lexer);

  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  keb_type_fn_t *fnt = malloc(sizeof(*fnt));
  fnt->param_types = list_init(LIST_START_SIZE, sizeof(keb_type_t));

  START_PARSING("fn_params");

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    keb_type_t *kt = parse_type(lexer);
    list_push_back(fnt->param_types, kt);
    free(kt);

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(TOKEN_COMMA, lexer);
    }
  }

  FINISH_PARSING("fn_params");

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  START_PARSING("fn_return_type");

  fnt->return_type = parse_type(lexer);

  FINISH_PARSING("fn_return_type");

  FINISH_PARSING("fn_type");

  return fnt;
}

static keb_type_list_t *parse_type_list(lexer_t *lexer) {
  // list types should look like `list(string)`, more advanced:
  // `list(fn(int, char) => string)`
  START_PARSING("list_type");

  SKIP_TOKEN(TOKEN_LIST, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  keb_type_list_t *klt = malloc(sizeof(*klt));
  klt->type = parse_type(lexer);

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("list_type");

  return klt;
}

static keb_type_t *parse_type(lexer_t *lexer) {
  START_PARSING("type");

  keb_type_t *kt = malloc(sizeof(*kt));

  switch (lexer->cur_token->kind) {
  case TOKEN_NAME:
    START_AND_FINISH_PARSING("type_primitive");
    kt->type = TYPE_PRIMITIVE;
    // TODO: maybe not strdup - i think necessary??
    kt->name = strdup(lexer->cur_token->name);
    lexer_advance(lexer);
    break;
  case TOKEN_FN:
    kt->type = TYPE_FN;
    kt->fn = parse_type_fn(lexer);
    break;
  case TOKEN_LIST:
    kt->type = TYPE_LIST;
    kt->list = parse_type_list(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  FINISH_PARSING("type");

  return kt;
}

static primitive_constructor_t *parse_primitive_constructor(lexer_t *lexer) {
  START_PARSING("bool_constructor");

  // NAME?
  SKIP_TOKEN(TOKEN_NAME, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  primitive_constructor_t *pc = malloc(sizeof(*pc));
  pc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    // list_push_back copies
    statement_t *stmt = parse_statement(lexer);
    list_push_back(pc->statements, stmt);
    free(stmt);
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("bool_constructor");

  return pc;
}

static fn_param_t *parse_fn_param(lexer_t *lexer) {
  START_PARSING("fn_param");

  fn_param_t *param = malloc(sizeof(*param));

  EXPECT_TOKEN(TOKEN_NAME, lexer);
  param->name = strdup(lexer->cur_token->name);
  lexer_advance(lexer);

  SKIP_TOKEN(TOKEN_COLON, lexer);

  param->type = parse_type(lexer);

  // Next token should be comma unless its the final param (then it should be
  // rparen, which will terminate the loop)
  if (lexer->cur_token->kind != TOKEN_RPAREN) {
    SKIP_TOKEN(TOKEN_COMMA, lexer);
  }

  FINISH_PARSING("fn_param");

  return param;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
  START_PARSING("fn_constructor");

  SKIP_TOKEN(TOKEN_FN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  fn_constructor_t *fnc = malloc(sizeof(*fnc));
  fnc->params = list_init(LIST_START_SIZE, sizeof(fn_param_t));

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    fn_param_t *fnp = parse_fn_param(lexer);
    list_push_back(fnc->params, fnp);
    free(fnp);
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  // parse function body
  fnc->body = parse_constructor(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  // TODO: make variadic and print debug info in finish_parsing/start_parsing
  FINISH_PARSING("fn_constructor");

  return fnc;
}

static list_constructor_t *parse_list_constructor(lexer_t *lexer) {
  START_PARSING("list_constructor");

  SKIP_TOKEN(TOKEN_LIST, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  list_constructor_t *lc = malloc(sizeof(*lc));
  lc->stmts = list_init(LIST_START_SIZE, sizeof(statement_t));
  lc->type = parse_type(lexer);

  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(lc->stmts, stmt);
    free(stmt);

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(TOKEN_COMMA, lexer);
    }
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

  FINISH_PARSING("list_constructor");

  return lc;
}

// TODO: modularize parser into multiple files, e.g. parse_constructor,
// parse_atom, parse_def, etc.
static constructor_t *parse_constructor(lexer_t *lexer) {
  START_PARSING("constructor");

  constructor_t *constr = malloc(sizeof(*constr));

  switch (lexer->cur_token->kind) {
  case TOKEN_NAME:
    constr->type = CONSTR_PRIMITIVE;
    constr->primitive_constructor = parse_primitive_constructor(lexer);
    break;

  case TOKEN_FN:
    constr->type = CONSTR_FN;
    constr->fn_constructor = parse_fn_constructor(lexer);
    break;

  case TOKEN_LIST:
    constr->type = CONSTR_LIST;
    constr->list_constructor = parse_list_constructor(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  FINISH_PARSING("constructor");

  return constr;
}

static atom_t *parse_atom(lexer_t *lexer) {
  // TODO: maybe free some strings here?
  atom_t *atom = malloc(sizeof(*atom));
  if (atom == NULL) {
    err_malloc_fail();
  }

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR_LITERAL:
    atom->type = ATOM_CHAR;
    atom->char_value = lexer->cur_token->char_literal;
    break;
  case TOKEN_STRING_LITERAL:
    atom->type = ATOM_STRING;
    atom->string_value = strdup(lexer->cur_token->string_literal);
    break;
  case TOKEN_INTEGER_LITERAL:
    atom->type = ATOM_INT;
    atom->int_value = lexer->cur_token->integer_literal;
    break;
  case TOKEN_NAME:
    atom->type = ATOM_NAME;
    atom->name = strdup(lexer->cur_token->name);
    break;
  // TODO: list, nil, bool, etc.
  default:
    err_illegal_token(lexer);
  }

  lexer_advance(lexer);

  START_AND_FINISH_PARSING("atom");

  return atom;
}

static primary_t *parse_primary(lexer_t *lexer) {
  START_PARSING("primary");

  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL) {
    lexer_free(lexer);
    err_malloc_fail();
  }

  prm->atom = parse_atom(lexer);

  // If next token is a left paren, parse arguments for function call.
  if (lexer->cur_token->kind == TOKEN_LPAREN) {
    if (prm->atom->type != ATOM_NAME) {
      // TODO: raise error (uncallable)
    }

    lexer_advance(lexer);

    prm->arguments = list_init(LIST_START_SIZE, sizeof(expression_t));
    while (lexer->cur_token->kind != TOKEN_RPAREN) {
      expression_t *expr = parse_expression(lexer);
      list_push_back(prm->arguments, expr);
      free(expr);

      if (lexer->cur_token->kind != TOKEN_RPAREN) {
        SKIP_TOKEN(TOKEN_COMMA, lexer);
      }
    }

    SKIP_TOKEN(TOKEN_RPAREN, lexer);
  } else {
    prm->arguments = NULL;
  }

  FINISH_PARSING("primary");

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
    break;
  default:
    uo = UNARY_NO_OP;
  }

#ifdef DEBUG
  if (uo != UNARY_NO_OP) {
    start_and_finish_parsing("factor_prefix");
  }
#endif

  return uo;
}

static factor_t *parse_factor(lexer_t *lexer) {
  START_PARSING("factor");

  factor_t *ft = malloc(sizeof(*ft));
  if (ft == NULL) {
    lexer_free(lexer); // TODO: add macro/function for this repetetive cleanup?
    err_malloc_fail(); // TODO: add malloc fail checks everywhere.
  }

  // Check for prefixes
  ft->prefix = parse_factor_prefix(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

  FINISH_PARSING("factor");

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
  if (bo != BINARY_NO_OP)
    start_and_finish_parsing("binary_operator");
#endif

  return bo;
}

// TODO: change condition to parse until not expr instead of newline/paren
static expression_t *parse_expression(lexer_t *lexer) {
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

static statement_t *parse_statement(lexer_t *lexer) {
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

ast_t *parse(lexer_t *lexer) {
  START_PARSING("ast");

  ast_t *ast = malloc(sizeof(*ast));
  ast->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_EOF) {
    statement_t *stmt = parse_statement(lexer);
    list_push_back(ast->statements, stmt);
    free(stmt);
  }

  FINISH_PARSING("ast");

  return ast;
}

// Cleanup functions

static void statement_free(void *stmt);
static void constructor_free(constructor_t *constr);
static void type_free(void *kt);
static void expression_free(void *expr);

/**
 * @param kt type to be freed, should be type `keb_type_t`
 */
static void type_free(void *kt) {
  keb_type_t *k = kt;

  switch (k->type) {
  case TYPE_PRIMITIVE:
    free((void *)k->name);
    break;
  case TYPE_LIST:
    type_free(k->list->type);
    free(k->list);
    break;
  case TYPE_FN:
    list_free(k->fn->param_types, type_free);
    type_free(k->fn->return_type);
    free(k->fn);
    break;
  }

  free(kt);
}

static void fn_param_free(void *fnp) {
  fn_param_t *f = fnp;
  free((void *)f->name);
  type_free(f->type);
  free(fnp);
}

// TODO: these free constructor functions are the same essentialy. Same other
// places. Should generalize
static void primitive_constructor_free(primitive_constructor_t *pc) {
  list_free(pc->statements, statement_free);
  free(pc);
}

static void fn_constructor_free(fn_constructor_t *fnc) {
  list_free(fnc->params, fn_param_free);
  constructor_free(fnc->body);
  free(fnc);
}

static void list_constructor_free(list_constructor_t *lc) {
  list_free(lc->stmts, statement_free);
  type_free(lc->type);
  free(lc);
}

static void constructor_free(constructor_t *constr) {
  switch (constr->type) {
  case CONSTR_PRIMITIVE:
    primitive_constructor_free(constr->primitive_constructor);
    break;

  case CONSTR_FN:
    fn_constructor_free(constr->fn_constructor);
    break;

  case CONSTR_LIST:
    list_constructor_free(constr->list_constructor);
    break;
  }

  free(constr);
}

static void atom_free(atom_t *atom) {
  switch (atom->type) {
  case ATOM_STRING:
    free((void *)atom->string_value);
    break;
  case ATOM_NAME:
    free((void *)atom->name);
    break;
  // TODO: list atom
  default:
    break;
  }

  free(atom);
}

static void primary_free(primary_t *prm) {
  atom_free(prm->atom);

  if (prm->arguments != NULL) {
    list_free(prm->arguments, expression_free);
  }

  free(prm);
}

/**
 * @param fac factor to free, should be type `factor_t`
 */
static void factor_free(void *fac) {
  factor_t *f = fac;
  primary_free(f->primary);
  free(f);
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
static void expression_free(void *expr) {
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
static void statement_free(void *stmt) {
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

void ast_free(ast_t *ast) {
  list_free(ast->statements, statement_free);
  free(ast);
}
