#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/parser.h"
#include "runtime/builtin.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"
#include "runtime/statements.h"
#include "runtime/types.h"

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
    printf("%.*s: %.*s\n", (int)entry->key->len, entry->key->s, (int)s->len, s->s);
  }

  scope_free(global_scope);
}

string_t *rt_value_to_string(const rt_value_t *v) {
  string_t *s = type_to_string(v->type);

  switch (v->type->kind) {
  case TYPE_CHAR:
    string_append_c(s, '(');
    string_append_c(s, v->char_value);
    string_append_c(s, ')');
    break;

  case TYPE_STRING:
    string_append(s, &string_of("(\""));
    string_append(s, v->string_value);
    string_append(s, &string_of("\")"));
    break;

  case TYPE_INT: {
    size_t int_slen = snprintf(NULL, 0, "%ld", v->int_value) + 1;
    char int_s[int_slen];
    snprintf(int_s, int_slen, "%ld", v->int_value);

    string_append_c(s, '(');
    string_append(s, &string_of(int_s));
    string_append_c(s, ')');
    break;
  }

  case TYPE_BOOL:
    string_append_c(s, '(');
    if (v->bool_value)
      string_append(s, &string_of("true"));
    else
      string_append(s, &string_of("false"));
    string_append_c(s, ')');
    break;

  case TYPE_LIST:
    string_append(s, &string_of("(["));
    for (size_t i = 0; i < v->list_value->size - 1; ++i) {
      string_t *cur_string = rt_value_to_string(list_get(v->list_value, i));
      string_append(s, cur_string);
      string_free(cur_string);

      // All list elements except the last one are succeeded by ", "
      if (i < v->list_value->size - 1)
        string_append(s, &string_of(", "));
    }
    string_append(s, &string_of(")]"));
    break;

  case TYPE_FN:
    // Don't need to do anything else, type_to_string is enough for functions
    break;
  }

  return s;
}
