#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "nonstdlib/nlist.h"
#include "nonstdlib/nstring.h"
#include "parser/constructors.h"
#include "parser/types.h"
#include "runtime/builtin.h"
#include "runtime/runtime.h"
#include "runtime/scope.h"

static void load_print(scope_t *scope) {}

static void load_open(scope_t *scope) {}

void load_globals(scope_t *global_scope) {
  load_print(global_scope);
  load_open(global_scope);
}
