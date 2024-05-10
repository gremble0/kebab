#include <stdlib.h>

#include "parser/logging.h"
#include "parser/types.h"

static keb_type_fn_t *parse_type_fn(lexer_t *lexer) {
  // function types should look something like `fn(int, int) => int`
  // for example:
  //     def fn-showcase = fn(some-param : fn(int, int) => int) => ...
  PARSER_LOG_NODE_START("fn-type");
  SKIP_TOKEN(lexer, TOKEN_FN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  keb_type_fn_t *fnt = malloc(sizeof(*fnt));
  fnt->param_types = list_init(LIST_START_SIZE); // list<keb_type_t *>

  PARSER_LOG_NODE_START("fn-params");

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnt->param_types, type_parse(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(lexer, TOKEN_COMMA);
    }
  }

  PARSER_LOG_NODE_FINISH("fn-params");
  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  PARSER_LOG_NODE_START("fn-return-type");

  fnt->return_type = type_parse(lexer);

  PARSER_LOG_NODE_FINISH("fn-return_type");
  PARSER_LOG_NODE_FINISH("fn-type");

  return fnt;
}

static keb_type_list_t *parse_type_list(lexer_t *lexer) {
  // list types should look like `list(string)`, more advanced:
  // `list(fn(int, char) => string)`
  PARSER_LOG_NODE_START("list-type");
  SKIP_TOKEN(lexer, TOKEN_LIST);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  keb_type_list_t *klt = malloc(sizeof(*klt));
  klt->type = type_parse(lexer);

  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  PARSER_LOG_NODE_FINISH("list-type");

  return klt;
}

keb_type_t *type_parse(lexer_t *lexer) {
  PARSER_LOG_NODE_START("type");

  keb_type_t *kt = malloc(sizeof(*kt));

  switch (lexer->cur_token->kind) {
  case TOKEN_CHAR:
    PARSER_LOG_NODE_SELF_CLOSING("type-char");
    kt->type = TYPE_CHAR;
    lexer_advance(lexer);
    break;

  case TOKEN_STRING:
    PARSER_LOG_NODE_SELF_CLOSING("type-string");
    kt->type = TYPE_STRING;
    lexer_advance(lexer);
    break;

  case TOKEN_INT:
    PARSER_LOG_NODE_SELF_CLOSING("type-int");
    kt->type = TYPE_INT;
    lexer_advance(lexer);
    break;

  case TOKEN_BOOL:
    PARSER_LOG_NODE_SELF_CLOSING("type-bool");
    kt->type = TYPE_BOOL;
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

  // TODO: TOKEN_NAME for user defined types/structs?
  default:
    err_illegal_token(lexer);
  }

  PARSER_LOG_NODE_FINISH("type");

  return kt;
}

/**
 * @param kt type to be freed, should be type `keb_type_t`
 */
void type_free(keb_type_t *kt) {
  switch (kt->type) {
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

  default:
    break;
  }

  free(kt);
}
