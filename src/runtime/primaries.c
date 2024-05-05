#include "runtime/primaries.h"
#include "nonstdlib/nerror.h"
#include "parser/types.h"
#include "runtime/atoms.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

// rt_value_t *eval_primary_arguments(

rt_value_t *eval_primary(primary_t *prm, scope_t *scope) {
  rt_value_t *v = eval_atom(prm->atom, scope);

  if (prm->arguments != NULL) {
    // Type error - only functions are callable
    if (v->type != TYPE_FN)
      ASSERT(0);

    // eval function call
  }

  return v;
}
