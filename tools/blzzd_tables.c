/*
 * Generate tables for blzzd's ntt
 *
 * Input: q, n, and psi such that
 * - (psi^2) is a primitive n-th root of unity modulo q
 * - q is less than 2^16
 *
 * First table:
 * w[i] = psi^i modulo q for i = 0 to n-1
 *
 * Second table:
 * r[i] = - (psi^i)/n modulo q, for i=0 to n-1
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
 * Print table w: w[i] = psi^i mod q for i=0 to n-1
 */
static void first_table(FILE *f, uint32_t n, uint32_t q, uint32_t psi) {
  uint32_t x, i, k;

  x = 1; // psi ^ 0
  k = 0;
  fprintf(f, "\nconst int32_t w%"PRIu32"n%"PRIu32"[%"PRIu32"] = {\n", q, n, n);
  for (i=0; i<n; i++) {
    if (k == 0) fprintf(f, "   ");
    fprintf(f, " %5"PRIu32",", x);
    k ++;
    if (k == 8) {
      fprintf(f, "\n");
      k = 0;
    }
    x = (x * psi) % q;
  }
  if (k > 0) fprintf(f, "\n");
  fprintf(f, "};\n\n");
}


/*
 * Second table r: r[i] = - (psi^n * inv_n) modulo q
 */
static void second_table(FILE *f, uint32_t n, uint32_t q, uint32_t psi, uint32_t inv_n) {
  uint32_t x, i, k;

  x = inv_n; // psi^0 * (1/n)
  k = 0;
  fprintf(f, "\nconst int32_t r%"PRIu32"n%"PRIu32"[%"PRIu32"] = {\n", q, n, n);
  for (i=0; i<n; i++) {
    if (k == 0) fprintf(f, "   ");
    fprintf(f, " %5"PRIu32",", q - x);
    k ++;
    if (k == 8) {
      fprintf(f, "\n");
      k = 0;
    }
    x = (x * psi) % q;
  }
  if (k > 0) fprintf(f, "\n");
  fprintf(f, "};\n\n");
}


int main(int argc, char *argv[]) {
  uint32_t q, psi, phi, n, i, inv_n;
  long x;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <modulus> <size> <psi>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  x = atol(argv[1]);
  if (x <= 1) {
    fprintf(stderr, "Invalid modulus %s: must be at least 2\n", argv[1]);
    exit(EXIT_FAILURE);    
  }
  if (x >= 0xFFFF) {
    fprintf(stderr, "The modulus is too large: max = %"PRIu32"\n", (uint32_t)0xFFFF);
    exit(EXIT_FAILURE);    
  }
  q = (uint32_t) x;

  x = atol(argv[2]);
  if (x <= 1) {
    fprintf(stderr, "Invalid size %s: must be at least 2\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  if (x >= 100000) {
    fprintf(stderr, "The size is too large: max = %"PRIu32"\n", (uint32_t)100000);
    exit(EXIT_FAILURE);
  }
  n = (uint32_t) x;

  x = atol(argv[3]);
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
  if (!inverse(n, q, &inv_n)) {
    fprintf(stderr, "invalid parameters: %"PRIu32" is not invertible modulo %"PRIu32"\n", n, q);
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "Parameters\n");
  fprintf(stderr, "q = %"PRIu32"\n", q);
  fprintf(stderr, "n = %"PRIu32"\n", n);
  fprintf(stderr, "psi = %"PRIu32"\n", psi);
  fprintf(stderr, "psi^2 = %"PRIu32"\n", phi);
  fprintf(stderr, "n^(-1) = %"PRIu32"\n", inv_n);
    
  first_table(stdout, n, q, psi);
  second_table(stdout, n, q, psi, inv_n);

  return 0;
}
