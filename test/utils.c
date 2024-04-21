#include <string.h>

#include "nonstdlib/nerror.h"

void assert_file_contents_equal(FILE *f1, FILE *f2) {
  char buffer1[BUFSIZ];
  char buffer2[BUFSIZ];

  while (!feof(f1) && !feof(f2)) {
    size_t read1 = fread(buffer1, 1, BUFSIZ, f1);
    size_t read2 = fread(buffer2, 1, BUFSIZ, f2);

    ASSERT(read1 == read2);

    ASSERT(memcmp(buffer1, buffer2, read1) == 0);
  }
}
