#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "list.h"
#include "parser.h"
#include "token.h"

static fn_constructor_t *parse_fn_constructor(ast_t *ast, lexer_t *lexer) {}

static constructor_t *parse_constructor(ast_t *ast, lexer_t *lexer) {
  constructor_t *constructor = malloc(sizeof(constructor_t));

  token_t *constructor_name = lexer_next_token(lexer);
  switch (constructor_name->kind) {
  case TOKEN_FN: {
    constructor->fn_constructor = parse_fn_constructor(ast, lexer);
    break;
  }
  default:
    err_illegal_token(constructor_name);
  }

  token_free(constructor_name);

  return constructor;
}

static definition_t *parse_definition(ast_t *ast, lexer_t *lexer) {
  definition_t *definition = malloc(sizeof(definition_t));

  token_t *symbol_token = lexer_next_token(lexer);
  definition->symbol = strdup(symbol_token->name);
  token_free(symbol_token);

  lexer_skip_token(lexer, TOKEN_EQUALS);

  definition->constructor = parse_constructor(ast, lexer);

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
    statement_t *statement = parse_statement(ast, lexer);
    printf("asd\n");
    if (statement == NULL) {
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
