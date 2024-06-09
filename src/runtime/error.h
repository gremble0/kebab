#pragma once

#include "parser/atoms.h"

_Noreturn void err_type_error(const char *expected, const char *actual);
_Noreturn void err_list_type_error(const char *expected, const char *actual);
_Noreturn void err_name_error(atom_t *name);
