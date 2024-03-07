#include <stdio.h>

#include "parser.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file.keb> <args>\n", argv[0]);
    return 1;
  }

  parser_parse(argv[1]);

  return 0;
}
