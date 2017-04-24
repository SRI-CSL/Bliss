#include <inttypes.h>
#include <stdio.h>

#include "modulii.h"

void testQ(int32_t q) {
  uint32_t index;
  uint32_t max;

  max = (q - 1) ^ 2;
  for (index = 0; index < max; index++) {
    int32_t m0 = index % q;
    int32_t m1 = modq(index, q);
    if (m0 != m1) {
      fprintf(stderr, "%" PRIu32 " != %" PRIu32 "\n", m0, m1);
    }
  }
}

void simpleTestQ(int32_t q) {
  int32_t index, m0, m1;

  for (index = INT32_MIN; index < INT32_MAX; index++) {
    m0 = index % q;
    m0 = m0 < 0 ? m0 + q : m0;

    m1 = smodq(index, q);
    if (m0 != m1) {
      fprintf(stderr, "%" PRIu32 " != %" PRIu32 "\n", m0, m1);
    }

    if (index % (1024 * 1024 * 256) == 0) {
      fprintf(stderr, ".");
    }
  }

  m0 = INT32_MAX % q;
  m0 = m0 < 0 ? m0 + q : m0;
  m1 = smodq(INT32_MAX, q);
  if (m0 != m1) {
    fprintf(stderr, "%" PRIu32 " != %" PRIu32 "\n", m0, m1);
  }

  fprintf(stderr, "!\n");
}

int main(int argc, char* argv[]) {
  testQ(7681);
  testQ(2 * 7681);
  testQ(12289);
  testQ(2 * 12289);
  simpleTestQ(7681);
  simpleTestQ(2 * 7681);
  simpleTestQ(12289);
  simpleTestQ(2 * 12289);
  return 0;
}
