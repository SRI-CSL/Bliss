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
#include "tests.h"
#include "ntt_microsoft.h"

#define PARAMETER_N 1024
#define PARAMETER_Q 12289

static void test_mul(void) {
  int32_t a[1024], b[1024], c[1024];
  uint32_t i;

  for (i=0; i<1024; i++) {
    a[i] = i;
    b[i] = i;
  }
  
  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();

    NTT_CT_std2rev_12289(a, psi_rev_ntt1024_12289, PARAMETER_N);
    NTT_CT_std2rev_12289(b, psi_rev_ntt1024_12289, PARAMETER_N);
    pmul(a, b, c, PARAMETER_N);
    correction(c, PARAMETER_Q, PARAMETER_N);
    INTT_GS_rev2std_12289(c, omegainv_rev_ntt1024_12289, omegainv7N_rev_ntt1024_12289, Ninv8_ntt1024_12289, PARAMETER_N);
    two_reduce12289(c, PARAMETER_N);
    correction(c, PARAMETER_Q, PARAMETER_N);
  }

  print_results("mul_microsoft1024: ", t, NTESTS);
}

int main(void){
  printf("\nTesting ntt_microsoft1024\n");
  test_mul();
  return 0;
}
