#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
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
static expression_t *parse_expr(lexer_t *lexer);

static int_constructor_t *parse_int_constructor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("int_constructor");
#endif

  SKIP_TOKEN(TOKEN_INT, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  int_constructor_t *intc = malloc(sizeof(*intc));
  intc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(intc->statements, parse_statement(lexer));
  }

  // TODO: if statements return type not int then error - something like this.
  // Do this in runtime or parser - parser looks hard.

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

#ifdef DEBUG
  finish_parsing("int_constructor");
#endif

  return intc;
}

static char_constructor_t *parse_char_constructor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("char_constructor");
#endif
  // TODO: very similar to int_constructor, maybe redo into one func with enum
  // as param or something

  SKIP_TOKEN(TOKEN_CHAR, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  char_constructor_t *chc = malloc(sizeof(*chc));
  chc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(chc->statements, parse_statement(lexer));
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

#ifdef DEBUG
  finish_parsing("char_constructor");
#endif

  return chc;
}

static string_constructor_t *parse_string_constructor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("string_constructor");
#endif
  // TODO: very similar to int_constructor, maybe redo into one func with enum
  // as param or something

  SKIP_TOKEN(TOKEN_STRING, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  string_constructor_t *strc = malloc(sizeof(*strc));
  strc->statements = list_init(LIST_START_SIZE, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(strc->statements, parse_statement(lexer));
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

#ifdef DEBUG
  finish_parsing("string_constructor");
#endif

  return strc;
}

static keb_type_fn_t *parse_type_fn(lexer_t *lexer) {
  // function types should look something like `fn(int, int) => int`
  // for example:
  //     def fn-showcase = fn(some-param : fn(int, int) => int) => ...
#ifdef DEBUG
  start_parsing("fn_type");
#endif

  SKIP_TOKEN(TOKEN_FN, lexer);

  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  keb_type_fn_t *fnt = malloc(sizeof(*fnt));
  fnt->param_types = list_init(LIST_START_SIZE, sizeof(keb_type_t));

#ifdef DEBUG
  start_parsing("fn_params");
#endif

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnt->param_types, parse_type(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(TOKEN_COMMA, lexer);
    }
  }

#ifdef DEBUG
  finish_parsing("fn_params");
#endif

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

#ifdef DEBUG
  start_parsing("fn_return_type");
#endif

  fnt->return_type = parse_type(lexer);

#ifdef DEBUG
  finish_parsing("fn_return_type");
#endif

#ifdef DEBUG
  finish_parsing("fn_type");
#endif

  return fnt;
}

static keb_type_list_t *parse_type_list(lexer_t *lexer) {
  // list types should look like `list(string)`
#ifdef DEBUG
  start_parsing("list_type");
#endif

  SKIP_TOKEN(TOKEN_LIST, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  keb_type_list_t *klt = malloc(sizeof(*klt));
  klt->type = parse_type(lexer);

  SKIP_TOKEN(TOKEN_RPAREN, lexer);

#ifdef DEBUG
  finish_parsing("list_type");
#endif

  return klt;
}

static keb_type_t *parse_type(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("type");
#endif

  keb_type_t *kt = malloc(sizeof(*kt));

  switch (lexer->cur_token->kind) {
    // TODO: debug info for primitive types
  case TOKEN_CHAR:
#ifdef DEBUG
    start_and_finish_parsing("type_char");
#endif
    kt->type = TYPE_CHAR;
    lexer_advance(lexer);
    break;
  case TOKEN_STRING:
#ifdef DEBUG
    start_and_finish_parsing("type_string");
#endif
    kt->type = TYPE_STRING;
    lexer_advance(lexer);
    break;
  case TOKEN_INT:
#ifdef DEBUG
    start_and_finish_parsing("type_int");
#endif
    kt->type = TYPE_INT;
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

#ifdef DEBUG
  finish_parsing("type");
#endif

  return kt;
}

static fn_param_t *parse_fn_param(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("fn_param");
#endif

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

#ifdef DEBUG
  finish_parsing("fn_param");
#endif

  return param;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("fn_constructor");
#endif

  SKIP_TOKEN(TOKEN_FN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  fn_constructor_t *fnc = malloc(sizeof(*fnc));
  fnc->params = list_init(LIST_START_SIZE, sizeof(fn_param_t));

  // parse params
  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnc->params, parse_fn_param(lexer));
  }

  SKIP_TOKEN(TOKEN_RPAREN, lexer);
  SKIP_TOKEN(TOKEN_FAT_RARROW, lexer);

  // parse function body
  fnc->body = parse_constructor(lexer);

  // Close the opening paren of the fn constructor
  SKIP_TOKEN(TOKEN_RPAREN, lexer);

#ifdef DEBUG
  // TODO: make variadic and print debug info in finish_parsing/start_parsing
  finish_parsing("fn_constructor");
#endif

  return fnc;
}

// TODO: modularize parser into multiple files, e.g. parse_constructor,
// parse_atom, parse_def, etc.
static constructor_t *parse_constructor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("constructor");
#endif

  constructor_t *constr = malloc(sizeof(*constr));
  // lexer_advance(lexer);

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR:
    constr->char_constructor = parse_char_constructor(lexer);
    break;

  case TOKEN_STRING:
    constr->string_constructor = parse_string_constructor(lexer);
    break;

  case TOKEN_INT:
    constr->int_constructor = parse_int_constructor(lexer);
    break;

  case TOKEN_FN:
    constr->fn_constructor = parse_fn_constructor(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

#ifdef DEBUG
  finish_parsing("constructor");
#endif

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

#ifdef DEBUG
  start_and_finish_parsing("atom");
#endif

  return atom;
}

static primary_t *parse_primary(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("primary");
#endif

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
      // TODO: maybe not parse_expr?
      list_push_back(prm->arguments, parse_expr(lexer));

      if (lexer->cur_token->kind != TOKEN_RPAREN) {
        SKIP_TOKEN(TOKEN_COMMA, lexer);
      }
    }

    SKIP_TOKEN(TOKEN_RPAREN, lexer);
  } else {
    prm->arguments = NULL;
  }

#ifdef DEBUG
  finish_parsing("primary");
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
    start_parsing("factor_prefix");
    finish_parsing("factor_prefix");
  }
#endif

  return uo;
}

static factor_t *parse_factor(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("factor");
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
  finish_parsing("factor");
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
  if (bo != BINARY_NO_OP)
    start_and_finish_parsing("binary_operator");
#endif

  return bo;
}

// TODO: change condition to parse until not expr instead of newline/paren
static expression_t *parse_expr(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("expr");
#endif

  expression_t *expr = malloc(sizeof(*expr));
  if (expr == NULL) {
    err_malloc_fail();
  }

  expr->factors = list_init(LIST_START_SIZE, sizeof(factor_t));
  // TODO: don't always init operators
  expr->operators = list_init(LIST_START_SIZE, sizeof(binary_operator_t));

  // First parse one factor and binary operator
  list_push_back(expr->factors, parse_factor(lexer));
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

      // Add the operator from the last iteration, and the factor for this
      // iteration
      list_push_back(expr->operators, bo_p);
      list_push_back(expr->factors, parse_factor(lexer));

      bo = parse_binary_operator(lexer); // operator for next iteration

      break;
    }

    default:
      err_illegal_token(lexer);
    }
  }

#ifdef DEBUG
  finish_parsing("expr");
#endif

  return expr;
}

// TODO: don't require constructor here, allow definition of primitives like
// `def a = 2` and infer type
static definition_t *parse_definition(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("definition");
#endif

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

#ifdef DEBUG
  finish_parsing("definition");
#endif

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("assignment");
#endif

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

#ifdef DEBUG
  finish_parsing("assignment");
#endif

  return ass;
}

static statement_t *parse_statement(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("statement");
#endif

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
    stmt->type = STMT_EXPRESSION;
    stmt->expr = parse_expr(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

#ifdef DEBUG
  finish_parsing("statement");
#endif

  return stmt;
}

ast_t *parse(lexer_t *lexer) {
#ifdef DEBUG
  start_parsing("ast");
#endif

  ast_t *ast = malloc(sizeof(*ast));
  ast->statements = list_init(10, sizeof(statement_t));

  while (lexer->cur_token->kind != TOKEN_EOF)
    list_push_back(ast->statements, parse_statement(lexer));

#ifdef DEBUG
  finish_parsing("ast");
#endif

  return ast;
}

void ast_free(ast_t *ast) {
  for (size_t i = 0; i < ast->statements->cur_size; ++i) {
    // TODO: tricky
    // make generic parser_free_statement() or something
    // needs to not free the statement itself as it will be freed by list_free()
  }

  // list_free(ast->statements);
  free(ast);
}
