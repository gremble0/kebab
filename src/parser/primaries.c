#include <stdlib.h>

#include "nonstdlib/nlist.h"
#include "parser/expressions.h"
#include "parser/primaries.h"
#include "parser/utils.h"

primary_t *parse_primary(lexer_t *lexer) {
  START_PARSING("primary");

  primary_t *prm = malloc(sizeof(*prm));
  if (prm == NULL) {
    lexer_free(lexer);
    err_malloc_fail();
  }

  prm->atom = parse_atom(lexer);

  // If next token is a left paren, parse arguments for function call.
  if (lexer->cur_token->kind == TOKEN_LPAREN) {
    // Here we could handle a small portion of wrong function calls (e.g. `1()`,
    // `""()`, etc), but we will leave them all to the runtime instead

    lexer_advance(lexer);

    prm->arguments = list_init(LIST_START_SIZE); // list<expression_t *>
    while (lexer->cur_token->kind != TOKEN_RPAREN) {
      list_push_back(prm->arguments, parse_expression(lexer));

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

void primary_free(primary_t *prm) {
  atom_free(prm->atom);

  if (prm->arguments != NULL) {
    list_map(prm->arguments, (list_map_func)expression_free);
    list_free(prm->arguments);
  }

  free(prm);
}
