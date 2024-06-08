#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nlist.h"
#include "parser/parser.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/statements.h"

void eval(ast_t *ast) {
  scope_t *global_scope = scope_init(NULL);

  for (size_t i = 0; i < ast->stmts->size; ++i)
    statement_eval(list_get(ast->stmts, i), global_scope);

  for (size_t i = 0; i < global_scope->bindings->capacity; ++i) {
    ht_entry_t *entry = global_scope->bindings->entries[i];
    if (entry == NULL) {
      printf("--empty--\n");
      continue;
    }

    string_t *s = rt_value_to_string(entry->value);
    printf("%s: %.*s\n", entry->key, (int)s->len, s->s);
  }

  scope_free(global_scope);
}

string_t *rt_value_to_string(const rt_value_t *v) {
  switch (v->type->kind) {
  case TYPE_CHAR: {
    size_t res_len = sizeof("char: ' '");
    char res[res_len];
    sprintf(res, "char: '%c'", v->char_value);
    return string_of(res, res_len);
  }
  case TYPE_STRING: {
    size_t res_len = sizeof("string: \"\"") + v->string_value->len;
    char res[res_len];
    sprintf(res, "string: \"%s\"", v->string_value->s);
    return string_of(res, res_len);
  }
  case TYPE_INT: {
    size_t res_len = snprintf(NULL, 0, "%ld", v->int_value) + sizeof("int: ");
    char res[res_len];
    sprintf(res, "int: %ld", v->int_value);
    return string_of(res, res_len);
  }
  case TYPE_BOOL:
    if (v->bool_value)
      return string_of("bool: true", sizeof("bool: true"));
    else
      return string_of("bool: false", sizeof("bool: false"));
  case TYPE_LIST:
    return string_of("list", sizeof("list"));
  case TYPE_FN:
    return string_of("fn", sizeof("fn"));
  }
}
