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
#include "poly.h"
#include "tests.h"
#include "ntt_blzzd.h"



#include "bliss_b_params.h"



int test_mul_from_KAT_values() {
  const bliss_param_t *p;

  int32_t *ua, *ub, *uc;

  // Include KAT vectors
  #include "data_poly_blzzd.c"

  p = &bliss_b_params[BLISS_B_1];


  ua = calloc(p->n, sizeof(int32_t));
  ub = calloc(p->n, sizeof(int32_t));
  uc = calloc(p->n, sizeof(int32_t));

  if (ua  != NULL  && ub != NULL &&  uc != NULL) {

    for (int i = 0; i < REPETITIONS; i++) {

      ntt32_xmu(ua, p->n, p->q, a[i], p->w);
      ntt32_fft(ua, p->n, p->q, p->w);

      ntt32_xmu(ub, p->n, p->q, b[i], p->w);
      ntt32_fft(ub, p->n, p->q, p->w);

      ntt32_xmu(uc, p->n, p->q, ua, ub);
      ntt32_fft(uc, p->n, p->q, p->w);

      ntt32_xmu(uc, p->n, p->q, uc, p->r);

      //ntt32_cmu(uc, p->n, p->q, uc, -1);    /* flip sign */
      ntt32_flp(uc, p->n, p->q);

      for (int j = 0; j < p->n; j++) {
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

  printf("\t Success.\n");
  return EXIT_SUCCESS;
}

int main(void){
  printf("\nTesting ntt from blzzd (KAT values)\n");

  return test_mul_from_KAT_values();
}
