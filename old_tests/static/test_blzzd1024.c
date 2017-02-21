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
#include "ntt_blzzd.h"
#include "blzzd1024_tables.h"


int test_mul_from_KAT_values() {
  int32_t *ua, *ub, *uc;

  // Include KAT vectors
  #include "data_poly_blzzd1024.c"

  ua = calloc(1024, sizeof(int32_t));
  ub = calloc(1024, sizeof(int32_t));
  uc = calloc(1024, sizeof(int32_t));

  if (ua  != NULL  && ub != NULL &&  uc != NULL) {

    for (int i = 0; i < REPETITIONS; i++) {

      ntt32_xmu(ua, 1024, 12289, a[i], w12289n1024);
      ntt32_fft(ua, 1024, 12289, w12289n1024);

      ntt32_xmu(ub, 1024, 12289, b[i], w12289n1024);
      ntt32_fft(ub, 1024, 12289, w12289n1024);

      ntt32_xmu(uc, 1024, 12289, ua, ub);
      ntt32_fft(uc, 1024, 12289, w12289n1024);

      ntt32_xmu(uc, 1024, 12289, uc, r12289n1024);

      ntt32_flp(uc, 1024, 12289);

      for (int j = 0; j < 1024; j++) {
        if (uc[j] != c[i][j]) {
          printf("\t Failure at round %d on coeff %d: %"PRIi32" != %"PRIi32".\n", i, j, uc[j], c[i][j]);
          return EXIT_FAILURE;
        }
      }
    }
  }

  free(ua);
  free(ub);
  free(uc);

  printf("\t Success after %d tests\n", REPETITIONS);
  return EXIT_SUCCESS;
}

int main(void){
  printf("\nTesting ntt from blzzd1024 (KAT values)\n");

  return test_mul_from_KAT_values();
}
