#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "lexer.h"
#include "list.h"
#include "parser.h"
#include "token.h"

static _Noreturn void err_illegal_token(token_t *token) {
  // TODO: write token_to_string() function for making errors
  enum token_kind_t token_kind = token->kind;
  token_free(token);
  err_custom("ERR_ILLEGAL_TOKEN: '%d'", token_kind);
}

static definition_t *parse_definition(ast_t *ast, lexer_t *lexer) {
  definition_t *definition = malloc(sizeof(definition_t));

  return definition;
}

static fn_call_t *parse_fn_call(ast_t *ast, lexer_t *lexer) {
  fn_call_t *fn_call = malloc(sizeof(fn_call_t));

  return fn_call;
}

static assignment_t *parse_assignment(ast_t *ast, lexer_t *lexer) {
  assignment_t *assignment = malloc(sizeof(assignment_t));

  return assignment;
}

static statement_t *parse_statement(ast_t *ast, lexer_t *lexer) {
  statement_t *statement = malloc(sizeof(statement_t));
  token_t *token = lexer_next_token(lexer);

  switch (token->kind) {
  case TOKEN_DEF:
    statement->definition = parse_definition(ast, lexer);
    break;

  case TOKEN_SET:
    statement->assignment = parse_assignment(ast, lexer);
    break;

  case TOKEN_NAME:
    statement->fn_call = parse_fn_call(ast, lexer);
    break;

  default:
    err_illegal_token(token);
  }

  token_free(token);

  return statement;
}

ast_t *parse_lexer(lexer_t *lexer) {
  ast_t *ast = malloc(sizeof(ast_t));
  ast->statements = list_init(10, sizeof(statement_t));

  // while (1) {
  //   list_push_back(ast->statements, parse_statement(ast, lexer));
  // }
  while (1) {
    token_t *token = lexer_next_token(lexer);

    if (token->kind == TOKEN_EOF) {
      token_free(token);
      break;
    } else {
      char *tok_string = token_to_string(token);
      printf("%s", tok_string);
      free(tok_string);
      token_free(token);
    }
  }

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
