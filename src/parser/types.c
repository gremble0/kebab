#include <stdlib.h>
#include <string.h>

#include "parser/types.h"
#include "parser/utils.h"

static keb_type_fn_t *parse_type_fn(lexer_t *lexer) {
  // function types should look something like `fn(int, int) => int`
  // for example:
  //     def fn-showcase = fn(some-param : fn(int, int) => int) => ...
  START_PARSING("fn_type");

  SKIP_TOKEN(TOKEN_FN, lexer);
  SKIP_TOKEN(TOKEN_LPAREN, lexer);

  keb_type_fn_t *fnt = malloc(sizeof(*fnt));
  fnt->param_types = list_init(LIST_START_SIZE); // list<keb_type_t *>

  START_PARSING("fn_params");

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnt->param_types, parse_type(lexer));

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

keb_type_t *parse_type(lexer_t *lexer) {
  START_PARSING("type");

  keb_type_t *kt = malloc(sizeof(*kt));

  switch (lexer->cur_token->kind) {
  case TOKEN_NAME:
    START_AND_FINISH_PARSING("type_primitive");
    kt->type = TYPE_PRIMITIVE;
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

/**
 * @param kt type to be freed, should be type `keb_type_t`
 */
void type_free(keb_type_t *kt) {
  switch (kt->type) {
  case TYPE_PRIMITIVE:
    free(kt->name);
    break;
  case TYPE_LIST:
    type_free(kt->list->type);
    free(kt->list);
    break;
  case TYPE_FN:
    list_map(kt->fn->param_types, (list_map_func)type_free);
    list_free(kt->fn->param_types);
    type_free(kt->fn->return_type);
    free(kt->fn);
    break;
  }

  free(kt);
}
