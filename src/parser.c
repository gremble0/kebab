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
  int_constructor_t *intc = malloc(sizeof(int_constructor_t));

  lexer_skip_token(lexer, TOKEN_LPAREN);
  intc->statements = list_init(3, sizeof(statement_t));

  list_push_back(intc->statements, parse_statement(lexer));

  // TODO: parse sequence of statements ...

  return intc;
}

static string_constructor_t *parse_string_constructor(lexer_t *lexer) {
  string_constructor_t *strc = malloc(sizeof(string_constructor_t));

  return strc;
}

static fn_constructor_t *parse_fn_constructor(lexer_t *lexer) {
  fn_constructor_t *fnc = malloc(sizeof(fn_constructor_t));

  return fnc;
}

static constructor_t *parse_constructor(lexer_t *lexer) {
  constructor_t *constr = malloc(sizeof(constructor_t));

  token_t *next_token = lexer_next_token(lexer);
  switch (next_token->kind) {
  case TOKEN_INT: {
    constr->int_constructor = parse_int_constructor(lexer);
    break;
  }
  case TOKEN_STRING: {
    constr->string_constructor = parse_string_constructor(lexer);
    break;
  }
  case TOKEN_FN: {
    constr->fn_constructor = parse_fn_constructor(lexer);
    break;
  }
  default:
    err_illegal_token(next_token);
  }

  token_free(next_token);

  return constr;
}
static primary_t *parse_primary(lexer_t *lexer) {
  primary_t *prm = malloc(sizeof(primary_t));
  if (prm == NULL) {
    lexer_free(lexer);
    err_malloc_fail();
  }

  return prm;
}

static operator_t parse_factor_prefix(lexer_t *lexer) {
  // TODO: REALLY SHOULD HAVE cur_token INSTEAD OF HAVING TO PEEK
  token_t *next = lexer_peek_token(lexer);
  token_kind_t next_kind = next->kind;
  token_free(next);

  if (next_kind == TOKEN_MINUS) {
    return OP_MINUS;
  } else if (next_kind == TOKEN_PLUS) {
    return OP_PLUS;
  } else {
    return OP_NONE;
  }
}

static operator_t parse_factor_suffix(lexer_t *lexer) {
  token_t *next = lexer_peek_token(lexer);
  token_kind_t next_kind = next->kind;
  token_free(next);

  if (next_kind == TOKEN_MINUS) {
    return OP_MINUS;
  } else if (next_kind == TOKEN_PLUS) {
    return OP_PLUS;
  } else if (next_kind == TOKEN_MULT) {
    return OP_MULT;
  } else if (next_kind == TOKEN_DIV) {
    return OP_DIV;
  } else {
    return OP_NONE;
  }
}

static factor_t *parse_factor(lexer_t *lexer) {
  factor_t *ft = malloc(sizeof(factor_t));
  if (ft == NULL) {
    lexer_free(lexer); // TODO: add macro/function for this repetetive cleanup?
    err_malloc_fail(); // TODO: add malloc fail checks everywhere.
  }

  // Check for prefixes
  ft->prefix = parse_factor_prefix(lexer);

  // Parse primary
  ft->primary = parse_primary(lexer);

  // TODO: Check for suffixes
  ft->suffix = parse_factor_suffix(lexer);

  return ft;
}

// TODO: change condition to parse until not expr instead of newline/paren
static expr_t *parse_expr(lexer_t *lexer, int wait_for_paren) {
  expr_t *expr = malloc(sizeof(expr_t));
  expr->factors = list_init(3, sizeof(factor_t));

  token_kind_t closing_kind = wait_for_paren ? TOKEN_RPAREN : TOKEN_EOL;

  token_t *next = lexer_next_token(lexer);
  while (next->kind != closing_kind) {
    switch (next->kind) {
    case TOKEN_INTEGER_LITERAL:
    case TOKEN_STRING_LITERAL:
    case TOKEN_NAME:
      list_push_back(expr->factors, parse_factor(lexer));
      break;

    default:
      err_illegal_token(next);
    }

    token_free(next);
    next = lexer_next_token(lexer);
  }

  return expr;
}

static definition_t *parse_definition(lexer_t *lexer) {
  lexer_skip_token(lexer, TOKEN_DEF);

  definition_t *def = malloc(sizeof(definition_t));
  token_t *next_token = lexer_next_token(lexer);

  // If the definition is `def mut ...` add is_mutable flag and load next
  // token, otherwise definition should be immutable
  if (next_token->kind == TOKEN_MUT) {
    def->is_mutable = 1;
    token_free(next_token);
    next_token = lexer_next_token(lexer);
  } else {
    def->is_mutable = 0;
  }

  if (next_token->kind != TOKEN_NAME) {
    err_illegal_token(next_token);
  }

  def->name = strdup(next_token->name);
  token_free(next_token);

  lexer_skip_token(lexer, TOKEN_EQUALS);

  def->constructor = parse_constructor(lexer);

  return def;
}

static assignment_t *parse_assignment(lexer_t *lexer) {
  assignment_t *assignment = malloc(sizeof(assignment_t));

  return assignment;
}

static statement_t *parse_statement(lexer_t *lexer) {
  statement_t *statement = malloc(sizeof(statement_t));
  token_t *token = lexer_peek_token(lexer);

  switch (token->kind) {
  case TOKEN_DEF:
    statement->definition = parse_definition(lexer);
    break;

  case TOKEN_SET:
    statement->assignment = parse_assignment(lexer);
    break;

  case TOKEN_NAME:
  case TOKEN_INTEGER_LITERAL:
  case TOKEN_STRING_LITERAL:
    statement->expr = parse_expr(lexer, 0);
    break;

  // TODO: Is this right?
  case TOKEN_EOF:
    free(statement);
    token_free(token);
    return NULL;

  default:
    err_illegal_token(token);
  }

  token_free(token);

  return statement;
}

ast_t *parse_lexer(lexer_t *lexer) {
  ast_t *ast = malloc(sizeof(ast_t));
  ast->statements = list_init(10, sizeof(statement_t));

  while (1) {
    statement_t *statement = parse_statement(lexer);
    printf("asd\n");
    if (statement == NULL) {
      lexer_free(lexer);
      break;
    }

    list_push_back(ast->statements, statement);
  }
  // while (1) {
  //   token_t *token = lexer_next_token(lexer);
  //
  //   if (token->kind == TOKEN_EOF) {
  //     token_free(token);
  //     break;
  //   } else {
  //     char *tok_string = token_to_string(token);
  //     printf("%s", tok_string);
  //     free(tok_string);
  //     token_free(token);
  //   }
  // }

  return ast;
}

void parse_free(ast_t *ast) {
  for (size_t i = 0; i < ast->statements->cur_size; ++i) {
    // TODO: tricky
    // make generic parser_free_statement() or something
    // needs to not free the statement itself as it will be freed by list_free()
  }

  list_free(ast->statements);
  free(ast);
}
