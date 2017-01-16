/*

  Tancrede: I added to the repository Harvey’s NTT (with a file
  tools/precomputation-ntt-harvey.sage to explain how the constants
  are generated), and a new test test_poly.c which uses known values
  (a[i]*b[i]=c[i]) and verifies that INTT(NTT(a[i])*NTT(b[i])) == c[i]
  with Harvey’s NTT. I believe similar tests should be possible with
  the others NTTs in the repository, although they do not output
  numbers in [0, PARAM_Q) so the test should be adapted.

*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpucycles.h"
#include "poly.h"
#include "tests.h"

static void test_mul(void) {
  poly a, b, c;
  uint32_t i;

  for (i=0; i<1024; i++) {
    a.coeffs[i] = i;
    b.coeffs[i] = i;
  }
  
  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();
    poly_ntt_harvey(&a);
    poly_ntt_harvey(&b);
    poly_pointwise_naive(&c, &a, &b);
    poly_invntt_harvey(&c);
  }

  print_results("mul_harvey: ", t, NTESTS);}

int main(void){
  printf("\nTesting poly_ntt_harvey\n");
  test_mul();
  return 0;
}
