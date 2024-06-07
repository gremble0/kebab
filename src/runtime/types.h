#pragma once

#include "parser/types.h"

void type_compare(keb_type_t *kt1, keb_type_t *kt2);
string_t *type_to_string(keb_type_t *type);
void type_register(keb_type_t *type);
