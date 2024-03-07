#include "parser.h"
#include "lexer.h"

ast_t *parser_parse(const char *path) {
  lexer_t *lexer = lexer_init(path);

  while (1) {
    token_t *token = lexer_next_token(lexer);

    if (token->kind == TOKEN_EOF) {
      token_free(token);
      break;
    } else {
      token_print(token);
      token_free(token);
    }
  }

  lexer_free(lexer);

  return NULL;
}
