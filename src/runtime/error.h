#pragma once

#include "parser/atoms.h"

_Noreturn void err_opaque_type_error(keb_type_kind_t expected, keb_type_kind_t actual);
_Noreturn void err_type_error(keb_type_t *expected, keb_type_t *actual);
_Noreturn void err_name_error(atom_t *name);
