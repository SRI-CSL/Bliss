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
#include "ntt_blzzd.h"
#include "blzzd1024_tables.h"

static void test_mul(void) {
  int32_t a[1024], b[1024], c[1024];
  uint32_t i;

  for (i=0; i<1024; i++) {
    a[i] = i;
    b[i] = i;
  }
  
  for (i=0; i<NTESTS; i++) {
    t[i] = cpucycles();

    ntt32_xmu(a, 1024, 12289, a, w12289n1024);
    ntt32_fft(a, 1024, 12289, w12289n1024);

    ntt32_xmu(b, 1024, 12289, b, w12289n1024);
    ntt32_fft(b, 1024, 12289, w12289n1024);

    ntt32_xmu(c, 1024, 12289, a, b);
    ntt32_fft(c, 1024, 12289, w12289n1024);

    ntt32_xmu(c, 1024, 12289, c, r12289n1024);

    ntt32_flp(c, 1024, 12289);
  }

  print_results("mul_blzzd1024: ", t, NTESTS);
}

int main(void){
  printf("\nTesting ntt_blzzd1024\n");
  test_mul();
  return 0;
}
