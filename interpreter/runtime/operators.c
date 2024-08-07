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

rt_value_t *operator_unary_plus_eval(const rt_value_t *v) {
  switch (v->type->kind) {
  case TYPE_INT: {
    rt_value_t *positive = malloc(sizeof(*positive));
    positive->type = type_int;
    positive->int_value = +v->int_value;
    return positive;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_unary_minus_eval(const rt_value_t *v) {
  switch (v->type->kind) {
  case TYPE_INT: {
    rt_value_t *positive = malloc(sizeof(*positive));
    positive->type = type_int;
    positive->int_value = -v->int_value;
    return positive;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_unary_not_eval(const rt_value_t *v) {
  switch (v->type->kind) {
  case TYPE_BOOL: {
    rt_value_t *negated = malloc(sizeof(*negated));
    negated->type = type_bool;
    negated->bool_value = !v->bool_value;
    return negated;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_INT:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_add_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      err_type_error(type_int, rhs->type, span);

    rt_value_t *added = malloc(sizeof(*added));
    added->type = type_int;
    added->int_value = lhs->int_value + rhs->int_value;
    return added;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_minus_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *subtracted = malloc(sizeof(*subtracted));
    subtracted->type = type_int;
    subtracted->int_value = lhs->int_value - rhs->int_value;
    return subtracted;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_mult_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *multiplied = malloc(sizeof(*multiplied));
    multiplied->type = type_int;
    multiplied->int_value = lhs->int_value * rhs->int_value;
    return multiplied;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_div_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *divided = malloc(sizeof(*divided));
    divided->type = type_int;
    divided->int_value = lhs->int_value / rhs->int_value;
    return divided;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_lt_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value < rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_le_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value <= rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_eq_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value == rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_neq_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value != rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_gt_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value > rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}

rt_value_t *operator_binary_ge_eval(const rt_value_t *lhs, const rt_value_t *rhs, span_t span) {
  switch (lhs->type->kind) {
  case TYPE_INT: {
    if (rhs->type != type_int)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = type_bool;
    compared->bool_value = lhs->int_value >= rhs->int_value;
    return compared;
  }

  case TYPE_CHAR:
  case TYPE_STRING:
  case TYPE_BOOL:
  case TYPE_LIST:
  case TYPE_FN:
    ASSERT(0);
  }
}
