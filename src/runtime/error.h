#pragma once

#include "parser/types.h"
#include "runtime/scope.h"

_Noreturn void err_opaque_type_error(keb_type_kind_t expected, keb_type_kind_t actual, span_t span);
_Noreturn void err_type_error(keb_type_t *expected, keb_type_t *actual, span_t span);
_Noreturn void err_name_error(string_t *name, span_t span, scope_t *scope);
_Noreturn void err_index_error(size_t size, size_t index, span_t span);
