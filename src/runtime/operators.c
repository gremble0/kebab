#include <stdlib.h>

#include "nonstdlib/nerror.h"
#include "parser/types.h"
#include "runtime/operators.h"
#include "runtime/runtime.h"

// TODO: operator overloading maybe? Also error handling for wrong types. This
// is very barebones atm

rt_value_t *eval_operator_add(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *added = malloc(sizeof(*added));
    added->type = TYPE_INT;
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

rt_value_t *eval_operator_minus(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *subtracted = malloc(sizeof(*subtracted));
    subtracted->type = TYPE_INT;
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

rt_value_t *eval_operator_mult(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *multiplied = malloc(sizeof(*multiplied));
    multiplied->type = TYPE_INT;
    multiplied->int_value = lhs->int_value - rhs->int_value;
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

rt_value_t *eval_operator_div(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *divided = malloc(sizeof(*divided));
    divided->type = TYPE_INT;
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

rt_value_t *eval_operator_lt(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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

rt_value_t *eval_operator_le(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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

rt_value_t *eval_operator_eq(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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

rt_value_t *eval_operator_neq(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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

rt_value_t *eval_operator_gt(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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

rt_value_t *eval_operator_ge(rt_value_t *lhs, rt_value_t *rhs) {
  switch (lhs->type) {
  case TYPE_CHAR:
    ASSERT(0);
  case TYPE_STRING:
    ASSERT(0);
  case TYPE_INT: {
    if (rhs->type != TYPE_INT)
      ASSERT(0);

    rt_value_t *compared = malloc(sizeof(*compared));
    compared->type = TYPE_BOOL;
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
