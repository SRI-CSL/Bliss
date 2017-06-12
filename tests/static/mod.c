#include <inttypes.h>
#include <stdio.h>
#include <assert.h>


/*
 * https://blogs.msdn.microsoft.com/devdev/2005/12/12/integer-division-by-constants/
 *
 * To avoid division instructions, compilers use the equalities:
 *    x%Q = x - (x/Q) * Q
 *    x/Q = (x * K) >> k
 * for a constant K that's ceiling(2^k/Q). Clang/gcc use
 * k=45 and K=2863078533.
 *
 * We can pick other values for k, since x is between 0 and
 * (Q-1)^2. The following values work.
 *
 *       k |     K
 *   ----------------------
 *      41 |  178942409
 *      42 |  357884817
 *      43 |  715769634
 *      44 | 1431539267
 *      45 | 2863078533

 *  for k = 41 & Q = 7681
 *  K = 286293876
 *  for k = 41 & 2Q 15362 (Q = 7681) 
 *  K = 143146938
 *  for k = 41 & Q = 12289
 *  K = 178942409
 *  for k = 41 & 2Q = 24578 (Q = 12289) 
 *  K = 89471205

 *
 *
 * also need for mod_p  if we do it this way.
 *
 * another way would be to make K and k part of the params struct and
 * pass them is as arguments to the function:
 *
 * uint32_t divq(int32_t x, int32_t q, uint32_t k, uint32_t K) { 
 *    return (((uint64_t) x) * K) >> k;
 * }
 *
 */
static int32_t divq(int32_t x, int32_t q) {
  switch(q){
  case 12289:   return (((uint64_t) x) * 178942409) >> 41;
  case 24578:   return (((uint64_t) x) * 89471205)  >> 41;
  case 7681:    return (((uint64_t) x) * 286293876) >> 41;
  case 15362:   return (((uint64_t) x) * 143146938) >> 41;
  default: return x / q;
  }
}

static int32_t modq(int32_t x, int32_t q) {
  assert(x < ((q - 1) ^ 2));
  return x - divq(x, q) * q;
}

// Variant: use a simple variant
static inline int32_t smodq(int32_t x, int32_t q){
  assert(q > 0);
  int32_t y = x % q;
  return y + ((y >> 31) & q); 
}

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
