// ntt32.c
// 09-Jun-15  Markku-Juhani O. Saarinen <m.saarinen@qub.ac.uk>

// Number-Theoretic Transforms on a max 31-bit q value

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ntt_blzzd.h"

#ifndef NDEBUG
static bool good_arg(int32_t v[], uint32_t n, int32_t q){
  uint32_t i;

  for (i = 0; i < n; i++){
    if (v[i] < 0 || v[i] >= q) return false;
  }

  return true;
}
#endif


// Compute x^n (mod q).
int32_t ntt32_pwr(int32_t x, int32_t n, int32_t q) {
  int32_t y;

  y = 1;
  if (n & 1)
    y = x;

  n >>= 1;

  while (n > 0) {
    x = (x * x) % q;
    if (n & 1)
      y = (x * y) % q;
    n >>= 1;
  }

  return y;
}


/*
 * FFT operation (forward and inverse).  
 *
 * BD: modified to use 32-bit arithmetic (don't use ntt32_muln), 
 * which is safe if q is less than 2^16.
 * Also forced intermediate results to be between 0 and q-1.
 */
static inline int32_t sub_mod(int32_t x, int32_t y, int32_t q) {
  x -= y;
  //  return x < 0 ? x + q : x;
  return x + ((x >> 31) & q);
}

static inline int32_t add_mod(int32_t x, int32_t y, int32_t q) {
  //  x += y;
  //  return x - q >= 0 ? x - q : x;
  x += y - q;
  return x + ((x >> 31) & q);
}


void ntt32_fft(int32_t v[], uint32_t n, int32_t q, const int32_t w[]) {
  uint32_t i, j, k, l;
  int32_t x, y;

  assert(good_arg(v, n, q));

  // bit-inverse shuffle
  j = n >> 1;
  for (i = 1; i < n - 1; i++) {       // 00..0 and 11..1 remain same
    if (i < j) {
      x = v[i];
      v[i] = v[j];
      v[j] = x;
    }
    k = n;
    do {
      k >>= 1;
      j ^= k;
    } while ((j & k) == 0);
  }

  // main loops
  l = n;         // BD: avoid division n/i in the loop
  for (i = 1; i < n; i <<= 1) {
    //    l = n / i;
    for (k = 0; k < n; k += i + i) {
      x = v[k + i];
      v[k + i] = sub_mod(v[k], x, q);
      v[k] = add_mod(v[k], x, q);
    }

    for (j = 1; j < i; j++) {
      y = w[j * l];
      for (k = j; k < n; k += i + i) {
        x = (v[k + i] * y) % q;
        v[k + i] = sub_mod(v[k], x, q);
        v[k] = add_mod(v[k], x, q);
      }
    }

    l >>= 1;
  }

  assert(good_arg(v, n, q));
}

// Elementwise vector product  v = t (*) u.
// BD: modified to use 32 bit arithmetic
void ntt32_xmu(int32_t v[], uint32_t n, int32_t q, const int32_t t[], const int32_t u[]) {
  uint32_t i;
  int32_t x;

  // multiply each element point-by-point
  for (i = 0; i < n; i++) {
    x = (t[i] * u[i]) % q;
    v[i] = x + ((x >> 31) & q); // v[i] = if x<0 then x+q else x
  }

  assert(good_arg(v, n, q));

}

// Multiply with a scalar  v = t * c.
// BD: modified to use 32 bit arithmetic
void ntt32_cmu(int32_t v[], uint32_t n, int32_t q, const int32_t t[], int32_t c) {
  uint32_t i;
  int32_t x;

  for (i = 0; i < n; i++) {
    x = (t[i] * c) % q;
    v[i] = x + ((x >> 31) & q); // v[i] = if x<0 then x+q else x
  }

  assert(good_arg(v, n, q));

}

// Flip the order.
// BD: removed normalization modulo q, except for v[0] since we assume 0 <= v[i] < q.
void ntt32_flp(int32_t v[], uint32_t n, int32_t q) {
  uint32_t i, j;
  int32_t x;

  assert(good_arg(v, n, q));

  for (i = 1, j = n - 1; i < j; i++, j--) {
    x = v[i];
    v[i] = v[j];
    v[j] = x;
  }

  // replace v[0] by q - v[0] if v[0] > 0, keep v[0] = 0 otherwise
  x = q & ((-v[0]) >> 31);
  v[0] = x - v[0];
  
  assert(good_arg(v, n, q));
}

