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

int test_mul_from_KAT_values() {
// Include KAT vectors
#include "data_poly.c"

  for (size_t i = 0; i < REPETITIONS; i++) {
    poly tmp;
    poly_ntt_harvey(&a[i]);
    poly_ntt_harvey(&b[i]);
    poly_pointwise_naive(&tmp, &a[i], &b[i]);
    poly_invntt_harvey(&tmp);

    for (size_t j = 0; j < PARAM_N; j++) {
      if (tmp.coeffs[j] != c[i].coeffs[j]) {
        printf("\t Failure at round %zu on coeff %zu.\n", i, j);
        return EXIT_FAILURE;
      }
    }
  }

  printf("\t Success.\n");
  return EXIT_SUCCESS;
}

int main(void){
  printf("\nTesting poly multiplication (KAT values)\n");

  return test_mul_from_KAT_values();
}
