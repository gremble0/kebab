#include <stdlib.h>

#include "parser/atoms.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

rt_value_t *eval_atom(atom_t *atom, scope_t *scope) {
  rt_value_t *v = malloc(sizeof(*v));

  switch (atom->type) {
  case ATOM_CHAR:
    v->char_value = atom->char_value;
    v->type = TYPE_CHAR;
    break;

  case ATOM_STRING:
    v->string_value = atom->string_value;
    v->type = TYPE_STRING;
    break;

  case ATOM_INT:
    v->int_value = atom->int_value;
    v->type = TYPE_INT;
    break;

  case ATOM_BOOL:
    v->bool_value = atom->bool_value;
    v->type = TYPE_BOOL;
    break;

  case ATOM_NAME:
    // Look up value in scope and return it

  case ATOM_INNER_EXPR:
    // Eval expression and return it

  case ATOM_LIST:
    v->list_value = atom->list_value;
    v->type = TYPE_LIST;
    break;
  }

  return v;
}
