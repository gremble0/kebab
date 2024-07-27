#include <stdlib.h>

#include "parser/logging.h"
#include "parser/types.h"
#include "utils/utils.h"

keb_type_t *type_char = &(keb_type_t){.kind = TYPE_CHAR};
keb_type_t *type_string = &(keb_type_t){.kind = TYPE_STRING};
keb_type_t *type_int = &(keb_type_t){.kind = TYPE_INT};
keb_type_t *type_bool = &(keb_type_t){.kind = TYPE_BOOL};
keb_type_t *type_unparametrized_list = &(keb_type_t){.kind = TYPE_LIST};

static keb_type_fn_t type_fn_parse(lexer_t *lexer) {
  // function types should look something like `fn(int, int) => int`
  // for example:
  //     def fn-showcase = fn(some-param : fn(int, int) => int) => ...
  PARSER_LOG_NODE_START("fn-type");
  SKIP_TOKEN(lexer, TOKEN_FN);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  keb_type_fn_t fnt;
  fnt.param_types = list_init(LIST_START_SIZE); // list<keb_type_t *>

  PARSER_LOG_NODE_START("fn-params");

  while (lexer->cur_token->kind != TOKEN_RPAREN) {
    list_push_back(fnt.param_types, type_parse(lexer));

    if (lexer->cur_token->kind != TOKEN_RPAREN) {
      SKIP_TOKEN(lexer, TOKEN_COMMA);
    }
  }

  PARSER_LOG_NODE_FINISH("fn-params");
  SKIP_TOKEN(lexer, TOKEN_RPAREN);
  SKIP_TOKEN(lexer, TOKEN_FAT_RARROW);
  PARSER_LOG_NODE_START("fn-return-type");

  fnt.return_type = type_parse(lexer);

  PARSER_LOG_NODE_FINISH("fn-return_type");
  PARSER_LOG_NODE_FINISH("fn-type");

  return fnt;
}

static keb_type_list_t type_list_parse(lexer_t *lexer) {
  // list types should look like `list(string)`, more advanced:
  // `list(fn(int, char) => string)`
  PARSER_LOG_NODE_START("list-type");
  SKIP_TOKEN(lexer, TOKEN_LIST);
  SKIP_TOKEN(lexer, TOKEN_LPAREN);

  keb_type_list_t klt;
  klt.type = type_parse(lexer);

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
    kt->kind = TYPE_CHAR;
    lexer_advance(lexer);
    break;

  case TOKEN_STRING:
    PARSER_LOG_NODE_SELF_CLOSING("type-string");
    kt->kind = TYPE_STRING;
    lexer_advance(lexer);
    break;

  case TOKEN_INT:
    PARSER_LOG_NODE_SELF_CLOSING("type-int");
    kt->kind = TYPE_INT;
    lexer_advance(lexer);
    break;

  case TOKEN_BOOL:
    PARSER_LOG_NODE_SELF_CLOSING("type-bool");
    kt->kind = TYPE_BOOL;
    lexer_advance(lexer);
    break;

  case TOKEN_FN:
    kt->kind = TYPE_FN;
    kt->fn_type = type_fn_parse(lexer);
    break;

  case TOKEN_LIST:
    kt->kind = TYPE_LIST;
    kt->list_type = type_list_parse(lexer);
    break;

  default:
    err_illegal_token(lexer);
  }

  PARSER_LOG_NODE_FINISH("type");

  return kt;
}

void type_free(keb_type_t *kt) {
  switch (kt->kind) {
  case TYPE_LIST:
    type_free(kt->list_type.type);
    break;

  case TYPE_FN:
    list_map(kt->fn_type.param_types, (list_map_func)type_free);
    list_free(kt->fn_type.param_types);
    type_free(kt->fn_type.return_type);
    break;

  default:
    break;
  }

  free(kt);
}
