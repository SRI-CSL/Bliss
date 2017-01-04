#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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
 * Given q and n, lists all possible values of psi such that
 * psi^2n is a primitive root of unity.
 */
int main(int argc, char* argv[]){
  uint32_t q, psi, phi, n, i, test, count;
  long x;
  
  if(argc != 3){
    fprintf(stderr, "Usage: %s <q (e.g. 12289)> <n (e.g. 512 1024)>\n", argv[0]);
    return 1;
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
  n = (uint32_t) x;
 
  count = 0;
  for (i = 1; i < q; i ++) {
    test = power(i, n, q);
    if (test == q - 1) {
      count ++;
      psi = i;
      phi = (psi * psi) % q;
      assert(power(phi, n, q) == 1);
      printf("solution %-3"PRIu32": psi = %6"PRIu32", phi = psi^2 = %6"PRIu32"\n", count, psi, phi);
    }
  }

  return 0;
}
