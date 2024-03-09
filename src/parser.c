#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "list.h"
#include "parser.h"
#include "token.h"

static int_constructor_t *parse_int_constructor(ast_t *ast, lexer_t *lexer) {
  int_constructor_t *int_constructor = malloc(sizeof(int_constructor_t));

  lexer_skip_token(lexer, TOKEN_LPAREN);

  return int_constructor;
}

static string_constructor_t *parse_string_constructor(ast_t *ast,
                                                      lexer_t *lexer) {
  string_constructor_t *string_constructor =
      malloc(sizeof(string_constructor_t));

  return string_constructor;
}

static fn_constructor_t *parse_fn_constructor(ast_t *ast, lexer_t *lexer) {
  fn_constructor_t *fn_constructor = malloc(sizeof(fn_constructor_t));

  return fn_constructor;
}

static constructor_t *parse_constructor(ast_t *ast, lexer_t *lexer) {
  constructor_t *constructor = malloc(sizeof(constructor_t));

  token_t *constructor_name = lexer_next_token(lexer);
  switch (constructor_name->kind) {
  case TOKEN_INT: {
    constructor->int_constructor = parse_int_constructor(ast, lexer);
    break;
  }
  case TOKEN_STRING: {
    constructor->string_constructor = parse_string_constructor(ast, lexer);
    break;
  }
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

static expr_t *parse_expr(ast_t *ast, lexer_t *lexer) {
  expr_t *expr = malloc(sizeof(expr_t));

  return expr;
}

static definition_t *parse_definition(ast_t *ast, lexer_t *lexer) {
  definition_t *definition = malloc(sizeof(definition_t));

  token_t *symbol_token = lexer_next_token(lexer);
  definition->name = strdup(symbol_token->name);
  token_free(symbol_token);

  lexer_skip_token(lexer, TOKEN_EQUALS);

  definition->constructor = parse_constructor(ast, lexer);

  return definition;
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
  case TOKEN_INTEGER_LITERAL:
  case TOKEN_STRING_LITERAL:
    statement->expr = parse_expr(ast, lexer);
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
