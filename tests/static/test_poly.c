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