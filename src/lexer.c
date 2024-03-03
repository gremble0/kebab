#include "lexer.h"
#include <stdlib.h>

lexer_t *lexer_init() {
  lexer_t *lexer = malloc(sizeof(lexer_t));

  return lexer;
}
