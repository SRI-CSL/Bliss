/*
 * Generate tables for bit-revese shuffle
 *
 * Input: an integer k for tables of size n=2^k
 *
 * Basic table:
 * bitrev[i] = reverse of i for i=0 to n-1
 *
 * Second table:
 * revpair = all pairs (i, reverse i) where i < reverse i.
 * nrevpairs = number of pairs in this table
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

/*
 * Bitreverse of i, interpreted as a k-bit integer
 */
static uint32_t reverse(uint32_t i, uint32_t k) {
  uint32_t x, b, j;

  x = 0;
  for (j=0; j<k; j++) {
    b = i & 1;
    x = (x<<1) | b;
    i >>= 1;
  }

  return x;
}


/*
 * Fill in table a with a[i] = reverse(i, k)
 * - n = 2^k
 */
static void init_rev_table(uint32_t *a, uint32_t n, uint32_t k) {
  uint32_t i;

  assert(n == ((uint32_t)1 << k));  
  for (i=0; i<n; i++) {
    a[i] = reverse(i, k);
  }
}


/*
 * Count the number of i such that i < a[i]
 */
static uint32_t rev_table_npairs(const uint32_t *a, uint32_t n) {
  uint32_t i, c;

  c = 0;
  for (i=0; i<n; i++) {
    c += (i < a[i]);
  }
  return c;
}


/*
 * Print table a
 */
static void print_bitrev_table(FILE *f, const uint32_t *a, uint32_t n) {
  uint32_t i, k;

  fprintf(f, "\nconst uint32_t bitrev%"PRIu32"[%"PRIu32"] = {\n", n, n);
  k = 0;
  for (i=0; i<n; i++) {
    if (k == 0) fprintf(f, "   ");
    fprintf(f, " %5"PRIu32",", a[i]);
    k ++;
    if (k == 8) {
      fprintf(f, "\n");
      k = 0;
    }
  }
  if (k > 0) fprintf(f, "\n");
  fprintf(f, "};\n\n");
}

/*
 * Print the pair tables for a
 */
static void print_bitrev_pair_table(FILE *f, const uint32_t *a, uint32_t n) {
  uint32_t s, i, k;

  s = rev_table_npairs(a, n);
  fprintf(f, "\n#define BITREV%"PRIu32"_NPAIRS %"PRIu32"\n", n, s);
  fprintf(f, "\nconst uint32_t bitrev%"PRIu32"_pair[BITREV%"PRIu32"_NPAIRS][2] = {\n", n, n);
  k = 0;

  for (i=0; i<n; i++) {
    if (i < a[i]) {
      if (k == 0) fprintf(f, "   ");
      fprintf(f, " { %5"PRIu32", %5"PRIu32" },", i, a[i]);
      k ++;
      if (k == 4) {
	fprintf(f, "\n");
	k = 0;
      }
    }
  }
  if (k > 0) fprintf(f, "\n");
  fprintf(f, "};\n\n");
}

int main(int argc, char *argv[]) {
  uint32_t n, k;
  uint32_t *table;
  long x;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s k (to generate tables of size 2^k\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  x = atol(argv[1]);
  if (x <= 0) {
    fprintf(stderr, "invalid size: k must be at least 1\n");
    exit(EXIT_FAILURE);
  }
  if (x > 28) {
    fprintf(stderr, "size too large: max k = 28\n");
    exit(EXIT_FAILURE);
  }
  k = (uint32_t)x;
  n = (uint32_t) 1 << k;

  table = (uint32_t*) malloc(n * sizeof(uint32_t));
  if (table == NULL) {
    fprintf(stderr, "failed to allocate table (of size %"PRIu32"\n", n);
    exit(EXIT_FAILURE);
  }

  init_rev_table(table, n, k);
  print_bitrev_table(stdout, table, n);
  print_bitrev_pair_table(stdout, table, n);
  free(table);

  return 0;
}

