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
#include <inttypes.h>

#include "cpucycles.h"
#include "tests.h"
#include "ntt_microsoft.h"

#define PARAMETER_N 1024
#define PARAMETER_Q 12289

int test_mul_from_KAT_values() {
  int32_t uc[PARAMETER_N];

  // Include KAT vectors
  #include "data_poly_blzzd1024.c"

  for (int i = 0; i < REPETITIONS; i++) {
    NTT_CT_std2rev_12289(a[i], psi_rev_ntt1024_12289, PARAMETER_N);
    NTT_CT_std2rev_12289(b[i], psi_rev_ntt1024_12289, PARAMETER_N);
    pmul(a[i], b[i], uc, PARAMETER_N);
    correction(uc, PARAMETER_Q, PARAMETER_N);
    INTT_GS_rev2std_12289(uc, omegainv_rev_ntt1024_12289, omegainv7N_rev_ntt1024_12289, Ninv8_ntt1024_12289, PARAMETER_N);
    two_reduce12289(uc, PARAMETER_N);
    correction(uc, PARAMETER_Q, PARAMETER_N);

    for (int j = 0; j < 1024; j++) {
      if (uc[j] != c[i][j]) {
	printf("\t Failure at round %d on coeff %d: %"PRIi32" != %"PRIi32".\n", i, j, uc[j], c[i][j]);
	return EXIT_FAILURE;
      }
    }
  }

  printf("\t Success after %d tests\n", REPETITIONS);
  return EXIT_SUCCESS;
}

int main(void){
  printf("\nTesting ntt_microsoft N=1024 (KAT values)\n");

  return test_mul_from_KAT_values();
}
