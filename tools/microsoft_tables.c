/*
 * Generate tables for blzzd's ntt
 *
 * Input: n, and psi such that
 * - (psi^2) is a primitive n-th root of unity modulo q=12289
 * - q is less than 2^16
 *
 * First table:
 * psi_rev[i] = inv(3) * psi^bit_reverse(i) for i=0 to n-1
 *
 * Second table:
 * omegainv_rev[i] = inv(3) * inv(psi)^bit_reverse(i) for i=0 to n-1
 */

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/*
 * x^k modulo q
 */
static uint32_t power(uint32_t x, uint32_t k, uint32_t q) {
  uint32_t y;

  assert(q > 0);

  y = 1;
  while (k != 0) {
    if ((k & 1) != 0) {
      y = (y * x) % q;
    }
    k >>= 1;
    x = (x * x) % q;
  }
  return y;
}


/*
 * Check whether n is invertible modulo q and return the inverse in *inv_n
 */
static bool inverse(uint32_t n, uint32_t q, uint32_t *inv_n) {
  int32_t r1, r2, u1, u2, v1, v2, g, x;

  // invariant: r1 = n * u1 + q * v1
  //            r2 = n * u2 + q * v2
  r1 = n; u1 = 1; v1 = 0;
  r2 = q; u2 = 0, v2 = 1;
  while (r2 > 0) {
    assert(r1 == (int32_t) n * u1 + (int32_t) q * v1);
    assert(r2 == (int32_t) n * u2 + (int32_t) q * v2);
    assert(r1 >= 0);
    g = r1/r2;

    x = r1; r1 = r2; r2 = x - g * r2;
    x = u1; u1 = u2; u2 = x - g * u2;
    x = v1; v1 = v2; v2 = x - g * v2;
  }

  // r1 is gcd(n, q) = n * u1 + q * v1
  if (r1 == 1) {
    u1 = u1 % (int32_t) q;
    if (u1 < 0) u1 += q;
    assert(((((int32_t) n) * u1) % (int32_t) q) == 1);
    *inv_n = u1;
    return true;
  } else {
    return false;
  }
}

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
 * Check that n is a power of two and return k such that n=2^k.
 */
static bool logtwo(uint32_t n, uint32_t *k) {
  uint32_t i;

  i = 0;
  while ((n & 1) == 0) {
    i ++;
    n >>= 1;
  }
  if (n == 1) {
    *k = i;
    return true;
  }
  return false;
}


/*
 * Reverse table a: swap a[i] and a[j] where j = bitreverse(i)
 * - n = size of the table (must be equal to 2^k)
 */
static void bit_reverse_table(uint32_t *a, uint32_t n, uint32_t k) {
  uint32_t i, j, x;

  assert(n == ((uint32_t) 1) << k);

  for (i=0; i<n; i++) {
    j = reverse(i, k);
    if (i < j) {
      x = a[i];
      a[i] = a[j];
      a[j] = x;
    }
  }
}


/*
 * Table of scaled powers: a[i] = inv(3) * g^i
 */
static void build_power_table(uint32_t *a, uint32_t n, uint32_t q, uint32_t inv_three, uint32_t g) {
  uint32_t x, i;

  //  x = inv_three;
  x = 1;
  for (i=0; i<n; i++) {
    a[i] = x;
    x = (x * g) % q;
  }
}

/*
 * Print table a:
 * - name = string to use as prefix in name<nnn>_<qqq>
 */
static void print_table(FILE *f, const char* name, uint32_t *a, uint32_t n, uint32_t q) {
  uint32_t i, k;

  k = 0;
  fprintf(f, "\nconst int32_t %s%"PRIu32"_%"PRIu32"[%"PRIu32"] = {\n", name, n, q, n);
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


int main(int argc, char *argv[]) {
  uint32_t q, three_inv, psi, phi, psi_inv, n, i, k;
  uint32_t *table;
  long x;

  q = 12289;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <size> <psi>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Read size
  x = atol(argv[1]);
  if (x <= 1) {
    fprintf(stderr, "Invalid size %s: must be at least 2\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  if (x >= 100000) {
    fprintf(stderr, "The size is too large: max = %"PRIu32"\n", (uint32_t)100000);
    exit(EXIT_FAILURE);
  }
  n = (uint32_t) x;
  if (!logtwo(n, &k)) {
    fprintf(stderr, "Invalid size: %"PRIu32" is not a power of two\n", n);
    exit(EXIT_FAILURE);
  }
  assert(n == ((uint32_t) 1) << k);

  // Read psi
  x = atol(argv[2]);
  if (x <= 1 || x >= q) {
    fprintf(stderr, "psi must be between 2 and %"PRIu32"\n", q-1);
    exit(EXIT_FAILURE);
  }
  psi = (uint32_t) x;
  phi = (psi * psi) % q;

  i = power(psi, n, q);
  if (i != q-1) {
    fprintf(stderr, "invalid psi: %"PRIu32" is not an n-th root of -1  (%"PRIu32"^n = %"PRIu32")\n", psi, psi, i);
    exit(EXIT_FAILURE);
  }
  assert(power(phi, n, q) == 1);
  for (i=1; i<n; i++) {
    if (power(psi, i, q) == 1) {
      fprintf(stderr, "invalid psi: psi^2 is not a primitive n-th root of unity (psi^2 = %"PRIu32")\n", phi);
      fprintf(stderr, "             (psi^2)^"PRIu32" = 1\n");
      exit(EXIT_FAILURE);
    }
  }

  if (!inverse(psi, q, &psi_inv)) {
    fprintf(stderr, "BUG: failed to compute the inverse of %"PRIu32"\n", psi);
    exit(EXIT_FAILURE);
  }
  assert((psi * psi_inv) % q == 1);

  if (!inverse(3, q, &three_inv)) {
    fprintf(stderr, "BUG: failed to compute the inverse of 3\n");
    exit(EXIT_FAILURE);
  }
  assert((psi * psi_inv) % q == 1);
  assert((3 * three_inv) % q == 1);


  fprintf(stderr, "Parameters\n");
  fprintf(stderr, "q = %"PRIu32"\n", q);
  fprintf(stderr, "n = %"PRIu32"\n", n);
  fprintf(stderr, "k = %"PRIu32"\n", k);
  fprintf(stderr, "inv(3) = %"PRIu32"\n", three_inv);
  fprintf(stderr, "psi = %"PRIu32"\n", psi);
  fprintf(stderr, "psi_inv = %"PRIu32"\n", psi_inv);
  fprintf(stderr, "psi^2 = %"PRIu32"\n", phi);

  table = (uint32_t *) malloc(n * sizeof(uint32_t));
  if (table == NULL) {
    fprintf(stderr, "failed to allocate table of size %"PRIu32"\n", n);
    exit(EXIT_FAILURE);
  }
  
  build_power_table(table, n, q, three_inv, psi);
  bit_reverse_table(table, n, k);
  print_table(stdout, "psi_rev_ntt", table, n, q);
   
  build_power_table(table, n, q, three_inv, psi_inv);
  bit_reverse_table(table, n, k);
  print_table(stdout, "omegainv_rev_ntt", table, n, q);

  free(table);

  return 0;
}
