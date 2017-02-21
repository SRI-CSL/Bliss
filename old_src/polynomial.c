#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
#include "entropy.h"
#include "polynomial.h"


/*
   Constructs a random polyomial

   - v: where the random polynomial is stored
   - n: the length of the polynomial
   - nz1: the number of coefficients that are +-1
   - nz2: the number of coefficients that are +-2
   - zero: whether v should be zeroed out first, if false, v is assumed to be zeroed out.
   - entropy: an initialized source of randomness

*/
void uniform_poly(int32_t v[], int n, int nz1, int nz2, bool zero, entropy_t *entropy)
{
  int i, j;
  uint64_t x;

  if (zero) {
    for (i = 0; i < n; i++)
      v[i] = 0;
  }

  i = 0;
  while (i < nz1) {
    x = entropy_random_uint64(entropy);  //iam: do we really need 64 bits of randomness? seems like overkill.
    // TL: clearly overkill here :) Several options:
    // - it does not matter so much (key generation is usually not the thing we really want to speed up)
    // - draw 16 bits of randomness (16>log(n)+1) and do the same
    // - for one "x" of 64-bit, do the loop 4 times before drawing randomness again
    j = (x >> 1) % n; // nb: uniform because n is a power of 2
    if (v[j] != 0)
      continue;
    v[j] = x & 1 ? 1 : -1;
    i++;
  }

  i = 0;
  while (i < nz2) {
    x = entropy_random_uint64(entropy); //iam: do we really need 64 bits of randomness? seems like overkill.
    // TL: idem
    j = (x >> 1) % n; // nb: uniform because n is a power of 2
    if (v[j] != 0)
      continue;
    v[j] = x & 1 ? 2 : -2;
    i++;
  }
}

static inline int32_t abs(int32_t x){
  return x < 0 ? -x : x;
}

void fprint_poly(FILE* fp, int32_t v[], int32_t n){
  int32_t i, c;
  bool first;
  assert(0 < n);

  first = true;

  for(i = 0; i < n; i++){
    c = v[i];
    if(c != 0){
      if(first){
        first = false;
        if(c < 0) fprintf(fp, " - ");
      } else {
        fprintf(fp, (c < 0) ? " - " : " + ");
      }
      if (c == 1 || c == -1){
        fprintf(fp, "x^%"PRIi32, i);
      } else {
        fprintf(fp, "%"PRIi32"x^%"PRIi32, abs(c), i);
      }
    }
  }
  fprintf(fp, "\n");

}
