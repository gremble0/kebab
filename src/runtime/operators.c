#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/types.h"
#include "runtime/error.h"
#include "runtime/operators.h"
#include "runtime/runtime.h"

// TODO: operator overloading maybe? Also error handling for wrong types. This
// is very barebones atm. Also memory management is not done here yet. Currently
// values are being copied and re-mallocd

// Memory management: I think we can make functions void and mutate params

rt_value_t *eval_operator_unary_plus(rt_value_t *v) {
  switch (v->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    rt_value_t *positive = malloc(sizeof(*positive));
    positive->type = type_int;
    positive->int_value = +v->int_value;
    return positive;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_unary_minus(rt_value_t *v) {
  switch (v->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    rt_value_t *positive = malloc(sizeof(*positive));
    positive->type = type_int;
    positive->int_value = -v->int_value;
    return positive;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}
rt_value_t *eval_operator_unary_not(rt_value_t *v) {
  switch (v->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT:
    ASSERT(0);
  case TYPE_BOOL: {
    rt_value_t *negated = malloc(sizeof(*negated));
    negated->type = type_bool;
    negated->bool_value = !v->bool_value;
    return negated;
  }
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_add(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      // TODO: ->s shit, should also make type_to_string
      err_type_error("TODO: type_to_string", "TODO: type_to_string");

    rt_value_t *added = malloc(sizeof(*added));
    added->type = type_int;
    added->int_value = lhs->int_value + rhs->int_value;
    return added;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_minus(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *subtracted = malloc(sizeof(*subtracted));
    subtracted->type = type_int;
    subtracted->int_value = lhs->int_value - rhs->int_value;
    return subtracted;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_mult(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *multiplied = malloc(sizeof(*multiplied));
    multiplied->type = type_int;
    multiplied->int_value = lhs->int_value * rhs->int_value;
    return multiplied;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_div(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *divided = malloc(sizeof(*divided));
    divided->type = type_int;
    divided->int_value = lhs->int_value / rhs->int_value;
    return divided;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_lt(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value < rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_le(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value <= rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_eq(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value == rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_neq(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value != rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_gt(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value > rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *eval_operator_binary_ge(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value >= rhs->int_value;
    return compared;
  }
  case TYPE_BOOL:
    ASSERT(0);
  case TYPE_LIST:
    ASSERT(0);
  case TYPE_FN:
    ASSERT(0);
  }
}
