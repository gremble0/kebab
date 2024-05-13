#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "runtime/statements.h"

void eval(ast_t *ast) {
  scope_t *scope = scope_init(NULL);

  for (size_t i = 0; i < ast->stmts->size; ++i)
    eval_statement(list_get(ast->stmts, i), scope);
}

// TODO: remove stinky strlen
// TODO: rename token->integer_literal -> token->int_literal/int_lit
char *rt_value_to_string(rt_value_t *v) {
  switch (v->type) {
  case TYPE_CHAR: {
    char *res = malloc(sizeof("rt-char: ' '") + 1);
    sprintf(res, "rt-char: '%c'", v->char_value);
    return res;
  }
  case TYPE_STRING: {
    char *res = malloc(sizeof("rt-string: \"\"") + v->string_value->len + 1);
    sprintf(res, "rt-string: \"%s\"", v->string_value->s);
    return res;
  }
  case TYPE_INT: {
    int len = snprintf(NULL, 0, "%ld", v->int_value);
    char *res = malloc(sizeof("rt-int: ") + len + 1);
    sprintf(res, "rt-int: %ld", v->int_value);
    return res;
  }
  case TYPE_BOOL:
    if (v->bool_value)
      return strdup("rt-bool: true");
    else
      return strdup("rt-bool: false");
  case TYPE_LIST:
    return strdup("rt-list");
  case TYPE_FN:
    return strdup("rt-fn");
  }
}
